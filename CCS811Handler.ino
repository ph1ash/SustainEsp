#include <Wire.h>

#include "SparkFunCCS811.h" //Click here to get the library: http://librarymanager/All#SparkFun_CCS811

//#define CCS811_ADDR 0x5B //Default I2C Address
#define CCS811_ADDR 0x5A //Alternate I2C Address

CCS811 mySensor(CCS811_ADDR);

/*using CCS811;
using CCS811Core;*/

void reinitCCS811()
{
  if (mySensor.begin() == false)
  {
    Serial.println(".begin() returned with an error.");
    while(1);
  }
}

void setCCS811Env(float temp, float humidity)
{
  CCS811Core::CCS811_Status_e res = mySensor.setEnvironmentalData(humidity, temp);
}

void initCCS811()
{
  Serial.println("CCS811 Initializing");

  Wire.begin(); //Inialize I2C Harware

  if (mySensor.begin() == false)
  {
    Serial.println(".begin() returned with an error.");
    while(1);
  }
}

bool handleCCS811Status(int stat)
{
  switch (stat){
      case CCS811Core::CCS811_Stat_ID_ERROR:
        Serial.println("Error on CCS811 read: ID_ERROR");
        return false; 
      case CCS811Core::CCS811_Stat_I2C_ERROR:
        Serial.println("Error on CCS811 read: I2C_ERROR");
        return false; 
      case CCS811Core::CCS811_Stat_INTERNAL_ERROR:
        Serial.println("Error on CCS811 read: INTERAL_ERROR");
        return false; 
      case CCS811Core::CCS811_Stat_GENERIC_ERROR:
        Serial.println("Error on CCS811 read: GENERIC_ERROR");
        return false; 
      case CCS811Core::CCS811_Stat_NUM:
      case CCS811Core::CCS811_Stat_SUCCESS:
      default:
        return true;
    }
}

void pollCCS811(ccs811Sensor_t * sensor)
{
  //Check to see if data is ready with .dataAvailable()
  if (mySensor.dataAvailable())
  {
    //If so, have the sensor read and calculate the results.
    //Get them later
    CCS811Core::CCS811_Status_e mySensorStatus = mySensor.readAlgorithmResults();

    if (handleCCS811Status(mySensorStatus)) 
    {
      sensor->eco2 = mySensor.getCO2();
      sensor->tvoc = mySensor.getTVOC();
    }
  }
  else
  {
    sensor->eco2 = -1;
    sensor->tvoc = -1;
  }
}
