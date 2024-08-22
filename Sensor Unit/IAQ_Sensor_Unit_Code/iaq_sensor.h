#ifndef IAQ_SENSOR_H
#define IAQ_SENSOR_H
#define BLE_MODE 2
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
//Bluetooth Low Energy  Librabries
#include <BLEDevice.h> 
#if BLE_MODE == 1
  #include <BLEServer.h>
  #include <BLEUtils.h>
  #include <BLE2902.h>
#endif
#if BLE_MODE == 2
  #include <BLEAdvertising.h>
#endif
#include <vector>
//-----------------------------------------------------------------------

// Definitions
//-----------------------------------------------------------------------
#define PMSA_SET 12
#define RXD2 18
#define TXD2 17

/*
#define TFT_I2C_POWER 21
#define TFT_CS 7
#define TFT_DC 39
#define TFT_RST 40
#define TFT_BACKLITE 45 
*/

#define SAMPLING_PERIOD 60000 // 1 minutes sampling period
#define SEALEVELPRESSURE_HPA (1013.25) // Constant 
#define MAX_CONNECTIONS 4
//-----------------------------------------------------------------------

// Bluetooth UUIDs
#define SERVICE_UUID         "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID1 "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CHARACTERISTIC_UUID2 "77376cda-d67f-4c80-981e-7f7cd032c684"
#define CHARACTERISTIC_UUID3 "3ea2d89f-5e58-454f-82ad-6fe705428db2"
#define CHARACTERISTIC_UUID4 "a0ea85f6-86ef-427b-8743-446f01fc3421"



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

#if BLE_MODE == 1
  typedef struct BLE_DEVICE
  {
    uint8_t id = 0;
    BLEClient* client = NULL;
    bool status = false;
    uint16_t conn_id = -1;
    std::string receivedString = "";
  }DEVICE;
#endif

#if BLE_MODE == 2
  #define  BLE_INTERVAL 5000
  #define SENSOR_ID '3'
  esp_ble_gap_ext_adv_params_t legacy_adv_params = {
      .type = ESP_BLE_GAP_SET_EXT_ADV_PROP_LEGACY_IND,
      .interval_min = 800,
      .interval_max = 800,
      .channel_map = ADV_CHNL_ALL,
      .own_addr_type = BLE_ADDR_TYPE_RANDOM,
      .filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
      .primary_phy = ESP_BLE_GAP_PHY_1M,
      .max_skip = 0,
      .secondary_phy = ESP_BLE_GAP_PHY_1M,
      .sid = 2,
      .scan_req_notif = false,
  };

  static uint8_t legacy_adv_data[] = {
          0x02, 0x01, 0x06, //Flags
          0x02, 0x0a, 0xeb, //Tx Power
          22, 0x09,       //Complete Local Name
          'I', 'A', 'Q', '_', 'S', 'e', 'n', 's', 'o', 'r', '_', SENSOR_ID, ';', //[20]
          '0', '0', '0', '0', '0', '0', '0', 0X0
  };

  static uint8_t legacy_scan_rsp_data[] = {
          22, 0x09,                                    // 6-7
          'I', 'A', 'Q', '_', 'S', 'e', 'n', 's', 'o', 'r', '_', SENSOR_ID, ';', //[20]
          '0', '0', '0', '0', '0', '0', '0', 0X0
  };

  uint8_t addr_legacy[6] = {0xc0, 0xde, 0x52, 0x00, 0x00, 0x03};


#endif

#endif