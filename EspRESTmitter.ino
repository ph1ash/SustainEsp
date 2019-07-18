#include <NTPClient.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>
#include "PhlashCredentials.h" // Custom Credentials Header

#define USE_SERIAL Serial

#define PST_OFFSET -25200l

ESP8266WiFiMulti WiFiMulti;
char macAddr[18];

void initEspRestmitter() {

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(M_SSID, M_PASS);

  Serial.println("ESPRESTmitter initalized");
  
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
  
  strcpy(macAddr, WiFi.macAddress().c_str());
}

int espPost(float temp, float humidity) {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    // Format the data buffer
    char * dataBuf = (char *) malloc(sizeof(char) * 64); // malloc 64 bytes for temp & humidity
    
    //sprintf(dataBuf, "temp=%0.2f&humidity=%0.2f&time=%s", temp, humidity, timeArray); // OLD STUFF
    sprintf(dataBuf, "sensor,deviceid=%s temperature=%0.2f,humidity=%0.2f", macAddr, temp, humidity);

    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    // configure traged server and url
    http.begin(M_SERVER_URL, M_HTTPS_FPRINT); //HTTPS (Server URL + HTTPS Cert Fingerprint
    // http.begin("http://test.com"); //HTTP

    // start connection and send HTTP header
    Serial.println(dataBuf);
    int httpCode = http.POST(dataBuf);
    free(dataBuf);

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      USE_SERIAL.printf("[HTTP] POST... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        USE_SERIAL.println(payload);
      }
    } else {
      USE_SERIAL.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
    return 0;
  }
  else
  {
    Serial.println("WiFi not connected...");
  }
  return 1;
}

int espDynamicPost(char* endPoint, float sensorValue)
{
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    
    HTTPClient http;

    Serial.print("[HTTP] begin...\n");

    char * dataBuf = (char *)malloc(sizeof(char)*256);
    
    sprintf(dataBuf, "%s/api/%s?appId=%s&value=%0.2f", M_SERVER_URL, endPoint, M_DEVICE_KEY, sensorValue);

    Serial.println(dataBuf);
    
    // configure traged server and url
    http.begin(dataBuf, M_HTTPS_FPRINT); //HTTPS (Server URL + HTTPS Cert Fingerprint)

    int httpCode = http.POST("");

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);

      // Check if server liked what we did
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
    return 0;
  }
  else
  {
    Serial.println("WiFi not connected...");
  }
  return 1;
}
