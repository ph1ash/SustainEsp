
#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

#include "DHT.h"

#define MINUTES(x) (x*60e3)

dhtSensor_t sensor;

void setup() {
  Serial.begin(115200);
  Serial.println("Sustain core spinning up...");

  initEspRestmitter();
  initDHT();
  
  //ESP.deepSleep(10e6);
}

void loop() {
  
  uint8_t result = readDHT(&sensor);
  // Check to see if it failed to read, if it did, cancel the loop 
  if (result) {
    delay(500);
    return;  
  }

  result = espPost(sensor.temperature, sensor.humidity);
  // WiFi isn't connected, retry
  if (result) {
    delay(500);
    return;
  }
  
  // Wait a few seconds between measurements.
  Serial.println("Deep sleeping...");
  delay(MINUTES(3));
}

int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}
