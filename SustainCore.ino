#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

#define MINUTES(x) (x*60e3)

typedef struct {
  float temperature;
  float humidity;
}dhtSensor_t;

typedef struct {
  float eco2;
  float tvoc;
}ccs811Sensor_t;


dhtSensor_t dhtSensor;
ccs811Sensor_t gasSensor;


void setup() {
  Serial.begin(115200);
  Serial.println("Sustain core spinning up...");
  
  initMqttClient();
  initDHT();
  initCCS811();
  
  //ESP.deepSleep(10e6);
}

void loop() {

  mqttPoll();

  uint8_t result = readDHT(&dhtSensor);
  pollCCS811(&gasSensor);
  
  // Check to see if it failed to read, if it did, restart the loop 
  if (result) {
    delay(500);
    return;  
  }
  
  mqttSetTopic("environment/temperature");
  mqttSendMsg(dhtSensor.temperature);
  mqttSetTopic("environment/humidity");
  mqttSendMsg(dhtSensor.humidity);
  
  if ((gasSensor.eco2 > 0.0) || (gasSensor.tvoc > 0.0))
  {
    mqttSetTopic("environment/eco2");
    mqttSendMsg(gasSensor.eco2);
    mqttSetTopic("environment/tvoc");
    mqttSendMsg(gasSensor.tvoc);
  }

  // Wasn't able to post something, retry
  if (result) {
    delay(500);
    return;
  }
  
  // Wait a few seconds between measurements.
  Serial.println("Deep sleeping...");
  delay(MINUTES(1));
}
