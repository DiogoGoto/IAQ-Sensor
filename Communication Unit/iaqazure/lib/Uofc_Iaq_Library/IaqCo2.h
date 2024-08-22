#ifndef _iaq_co2_h
#define _iaq_co2_h
#include <Wire.h>
#include <SPI.h>
//#include <Adafruit_Sensor.h>
#include <Adafruit_SCD30.h>
//#include "IaqData.h"

class IaqCo2
{
public:
    Adafruit_SCD30 scd30; 
      // scd30.temperature;
      // scd30.relative_humidity;
      // scd30.CO2;
    bool isok = false;  // is opened;
    bool setup()
    {
      if(!isok)
      {
        isok = scd30.begin();
        if(isok)
        {
            //Serial.println("SCD30::SCD30 Found!");
            // if (!scd30.setMeasurementInterval(10)){
            //   Serial.println("Failed to set measurement interval");
            //   while(1){ delay(10);}
            // }
            //Serial.println("<+>SCD30 setup done.");
            Serial.print("+>SCD30::Measurement Interval: ");  
            Serial.print(scd30.getMeasurementInterval());   
            Serial.println(" seconds");
        }
        else
        {
            Serial.println("_>SCD30::Failed to find SCD30 chip");
            //while (1) { delay(10); }
        }
      }
      return isok;
    }
    
    bool read()
    {      
      if(!setup()) return false;

      if (scd30.dataReady())
      {
          //Serial.println("SCD30::Data available!");
          if (scd30.read()) 
          {
            //Serial.println("+>SCD30, read sensor data done"); 
            return true; 
          }
          else{
            isok=false; //2024-01-02;
            Serial.println("_>SCD30::Error reading sensor data"); 
          }
      }
      else return false;
    }
};

#endif