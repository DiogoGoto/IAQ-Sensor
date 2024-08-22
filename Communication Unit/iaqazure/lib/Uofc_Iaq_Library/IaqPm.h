#ifndef _iaq_pm_h
#define _iaq_pm_h
#include <Wire.h>
#include <SPI.h>
#include "Adafruit_PM25AQI.h"
//#include "IaqData.h"
// lib = Adafruit PM25 AQI Sensor

// If your PM2.5 is UART only, for UNO and others (without hardware serial) 
// we must use software serial...
// pin #2 is IN from sensor (TX pin on sensor), leave pin #3 disconnected
// comment these two lines if using hardware serial
//#include <SoftwareSerial.h>
//SoftwareSerial pmSerial(2, 3);

class IaqPm
{
public:
    Adafruit_PM25AQI aqi= Adafruit_PM25AQI();;
    PM25_AQI_Data data;

    bool isok = false;

    bool setup()
    {
      if(!isok)
      {
        // There are 3 options for connectivity!
        isok = aqi.begin_I2C();
        if(!isok)
        { 
            //if (! aqi.begin_UART(&Serial1)) { // connect to the sensor over hardware serial
            //if (! aqi.begin_UART(&pmSerial)) { // connect to the sensor over software serial 
            Serial.println("_>Could not find PM 2.5 sensor.");
            //while (1) delay(10);
        }
        else
        {
            Serial.println("+>PM25 found!");
        }
      }
      return isok;
    }
    
  bool read() 
  {
    if(!setup()) return false;

    isok = aqi.read(&data);
    if (isok) 
    {
      //Serial.println("+>AQI reading success");
      //Serial.print(F("PM 1.0: ")); Serial.print(data.pm10_standard);
      //Serial.print(F("\t\tPM 2.5: ")); Serial.print(data.pm25_standard);
      //Serial.print(F("\t\tPM 10: ")); Serial.println(data.pm100_standard);

      // iaq_data.pm10_standard=data.pm10_standard;
      // iaq_data.pm25_standard=data.pm25_standard;
      // iaq_data.pm100_standard=data.pm100_standard;
      // iaq_data.pm_03um=data.particles_03um;
      // iaq_data.pm_05um=data.particles_05um;
      // iaq_data.pm_10um=data.particles_10um;
      // iaq_data.pm_25um=data.particles_25um;
      // iaq_data.pm_50um=data.particles_50um;
      // iaq_data.pm_100um=data.particles_100um;
    } 
    else
    {
        Serial.println("_>Failed to read from PM2.5 data");
        //delay(500);  // try again in a bit!
        //return;
    }
    return isok;
  }

  void setupPM25(void) 
  {
      // ispmsetup=false;
      Serial.println("Adafruit PMSA003I Air Quality Sensor");

      // There are 3 options for connectivity!
      if (! aqi.begin_I2C()) {      // connect to the sensor over I2C
      //if (! aqi.begin_UART(&Serial1)) { // connect to the sensor over hardware serial
      //if (! aqi.begin_UART(&pmSerial)) { // connect to the sensor over software serial 
        Serial.println("Could not find PM 2.5 sensor!");
        //while (1) delay(10);
        return;
      }
      
      // ispmsetup=true;
      Serial.println("PM25 found!");
  }
  void loopPM25() 
  {
    if (!aqi.read(&data)) {
      Serial.println("Could not read from AQI");
      //delay(500);  // try again in a bit!
      return;
    }
    Serial.println("AQI reading success");

    // Serial.println();
    // Serial.println(F("---------------------------------------"));
    // Serial.println(F("Concentration Units (standard)"));
    // Serial.println(F("---------------------------------------"));
    Serial.print(F("PM 1.0: ")); Serial.print(data.pm10_standard);
    Serial.print(F("\t\tPM 2.5: ")); Serial.print(data.pm25_standard);
    Serial.print(F("\t\tPM 10: ")); Serial.println(data.pm100_standard);
    // Serial.println(F("Concentration Units (environmental)"));
    // Serial.println(F("---------------------------------------"));
    // Serial.print(F("PM 1.0: ")); Serial.print(data.pm10_env);
    // Serial.print(F("\t\tPM 2.5: ")); Serial.print(data.pm25_env);
    // Serial.print(F("\t\tPM 10: ")); Serial.println(data.pm100_env);
    // Serial.println(F("---------------------------------------"));
    // Serial.print(F("Particles > 0.3um / 0.1L air:")); Serial.println(data.particles_03um);
    // Serial.print(F("Particles > 0.5um / 0.1L air:")); Serial.println(data.particles_05um);
    // Serial.print(F("Particles > 1.0um / 0.1L air:")); Serial.println(data.particles_10um);
    // Serial.print(F("Particles > 2.5um / 0.1L air:")); Serial.println(data.particles_25um);
    // Serial.print(F("Particles > 5.0um / 0.1L air:")); Serial.println(data.particles_50um);
    // Serial.print(F("Particles > 10 um / 0.1L air:")); Serial.println(data.particles_100um);
    // Serial.println(F("---------------------------------------"));
    

    //delay(1000);
  }
};

#endif