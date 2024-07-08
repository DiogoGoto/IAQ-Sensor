#ifndef IAQ_SENSOR_H
#define IAQ_SENSOR_H
// Includes
//-----------------------------------------------------------------------
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_GFX.h>     // Core graphics library
#include "Adafruit_ST7789.h"  // Hardware-specific library for ST7789
#include <Adafruit_SCD30.h>   // CO2 Sensor
#include <Adafruit_PM25AQI.h> // PM Sensor
#include <HardwareSerial.h>   // Inteface with Dr. Sheng's Part
#include "iaqbsec.h"          // VOC & Pressure Sensor 
//-----------------------------------------------------------------------

// Definitions
//-----------------------------------------------------------------------
#define PMSA_SET 12
#define RXD2 16
#define TXD2 17

#define SAMPLING_PERIOD 60000 // 1 minutes sampling period
#define SEALEVELPRESSURE_HPA (1013.25) // Constant 
//-----------------------------------------------------------------------


// Colors definitions
// Colors are represented by 16bits values
//-----------------------------------------------------------------------
#define BLACK 0x0000
#define WHITE 0xFFFF
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define ORANGE 0xFC00
//-----------------------------------------------------------------------

typedef struct IAQ_DATA
{
  float sensorid = 3;
  
  //SCD 30 Data
  float temperature = 0.0;
  float humidity = 0.0;
  float co2 = 0.0;

  //PMSA 0031 Data
  float pm1dot0 = 0.0;
  float pm2dot5 = 0.0;
  float pm10 = 0.0;

  //BME 688 Data
  float gasTemperature = 0.0;
  float gasPressure = 0.0;
  float gasHumidity = 0.0;
  float gasResistance = 0.0;
  float gasIaq=0.0;
  float gaseVoc=0.0;
  float gaseCo2=0.0;
}iaqData;

#endif