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

  initEspRestmitter();
  initDHT();
  initCCS811();
  
  //ESP.deepSleep(10e6);
}

void loop() {
  
  uint8_t result = readDHT(&dhtSensor);
  pollCCS811(&gasSensor);
  // Check to see if it failed to read, if it did, cancel the loop 
  if (result) {
    delay(500);
    return;  
  }

  //result = espPost(sensor.temperature, dhtSensor.humidity);
  result = espDynamicPost("temperature", dhtSensor.temperature);
  result = result & espDynamicPost("humidity", dhtSensor.humidity);
  if ((gasSensor.eco2 > 0.0) || (gasSensor.tvoc > 0.0))
  {
    result = result & espDynamicPost("eco2", gasSensor.eco2);
    result = result & espDynamicPost("tvoc", gasSensor.tvoc);  
  }

  // Wasn't able to post something, retry
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
