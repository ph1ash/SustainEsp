#include <NTPClient.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
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

int espDynamicPost(char* endPoint, float sensorValue)
{
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    
    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
    
    client->setFingerprint(M_FPRINT);
  
    HTTPClient https;
  
    Serial.print("[HTTP] begin...\n");

    char * dataBuf = (char *)malloc(sizeof(char)*256);
    
    sprintf(dataBuf, "%s/api/%s?appId=%s&value=%0.2f", M_SERVER_URL, endPoint, M_DEVICE_KEY, sensorValue);

    Serial.println(dataBuf);
    
    // configure traged server and url
    https.begin(*client, dataBuf); //HTTPS (Server URL + HTTPS Cert Fingerprint
    
    int httpCode = https.POST("");

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);

      // Check if server liked what we did
      if (httpCode == HTTP_CODE_OK) {
        String payload = https.getString();
        Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", https.errorToString(httpCode).c_str());
    }
    
    free(dataBuf);
    https.end();
    return 0;
  }
  else
  {
    Serial.println("WiFi not connected...");
  }
  return 1;
}
