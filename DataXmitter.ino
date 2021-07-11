#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ArduinoMqttClient.h>
#include "PhlashCredentials.h" // Custom Credentials Header

ESP8266WiFiMulti WiFiMulti;
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);
char macAddr[18];

const char broker[] = M_SERVER_URL;
int        port     = M_SERVER_PORT;
char       topic[100];

void connectToWifi() {
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(M_SSID, M_PASS);
  
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
  
  strcpy(macAddr, WiFi.macAddress().c_str());
  
  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.println("WiFi not connected...");
    delay(500);
  }
}

void initMqttClient() {
  mqttClient.setKeepAliveInterval(MINUTES(5));
  if (!mqttClient.connect(broker, port)) {
      Serial.print("MQTT connection failed! Error code = ");
      Serial.println(mqttClient.connectError());
  }

  Serial.println("Connected to the MQTT broker");
}

void reconnectMqttClient() {
  mqttClient.stop();
  initMqttClient();
}

void mqttPoll() {
  mqttClient.poll();
}

bool mqttIsConnected() {
  return mqttClient.connected();
}

void mqttSendMsg(float data) {
  mqttClient.beginMessage(topic);
  mqttClient.print(data);
  mqttClient.endMessage();
}

void mqttSetTopic(char* newTopic) {
  Serial.print("Setting topic to: ");
  Serial.println(topic);
  strcpy(topic, newTopic);
}
