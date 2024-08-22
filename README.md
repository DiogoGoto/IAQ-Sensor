# Indoor Air Quality (IAQ) Sensor

## Project Description
Low-cost indoor air quality sensor, this sensor is capable of collecting various air quality parameters and sending this data to a database hosted by Microsoft Azure, where the data can be accessed by a website. This data is also broadcasted using Bluetooth and it can be collected using an app developed using MIT App Inventor, with this app, and multiple sensors, it is also possible to keep track of where an individual has been and collect the personal air quality. More info at [Project post](https://www.hackster.io/Diterragames/indoor-air-quality-sensors-iaq-27ae14#overview)

## Table of Contents
- [Project Description](#project-description)
- [Functionality](#functionality)
- [Components](#components)
- [Quick Setup](#quick-setup)
- [Libraries](#libraries)
- [Contact](#contact)


## Functionality
- Measures Temperature (Celsius)
- Measures Relative Humidity (%)
- Measures CO2 (PPM)
- Measures Pressure (hPa) 
- Measures VOCs (PPM)
- Measures PM 1.0, 2.5, 10 (ug/m3)
- Sends data through Bluetooth
- Sends data to an online database
- Website to visualize the data



## Components
- [ESP-32 S3 Reverse TFT Feather](https://learn.adafruit.com/esp32-s3-reverse-tft-feather/overview)
- [ESP-32 S2 TFT Feather](https://learn.adafruit.com/adafruit-esp32-s2-tft-feather/overview)
- [SCD 30](https://learn.adafruit.com/adafruit-scd30/overview)
- [PMSA0031](https://learn.adafruit.com/pmsa003i/overview)
- [BME688](https://learn.adafruit.com/adafruit-bme680-humidity-temperature-barometic-pressure-voc-gas/overview)
- [Stemma QT cable 100mm](https://www.adafruit.com/product/4210)
- [Lipo 1200mAh Battery](https://www.adafruit.com/product/258)

## Quick Setup
Start by cloning this repository into your machine
#### Case and Circuit
1. Print the enclosure files under CAD Files.
2. Solder the sensors to the perfboard.
3. Connect the components as shown in the schematic.
4. Put the components inside their cases.
#### Cloud
1. Create a resource
2. Create an Azure IoT device using symmetric key as authentication type.
3. Save the _Device ID_ and _Primary Key_.
4. Create a CosmosDB account.
5. Create a CosmosDB routing endpoint.
6. Install Python 3.9.
7. Inside the project folder, run the following command: pip install -r requirements.txt.
8. Open cosmosdb.py and add your endpoint, database name and container name.
9. Run server.py.
10. Open azure_client.html.
#### Communication Unit Code
1. Install the esp32 board manager on version 2.0.17 by Espressif.
2. Install the [libraries](#libraries) for the communication unit.
3. Copy the folder _Uofc_Iaq_Library_ located Communication Unit\iaqazure\lib into your Arduino Libraries folder.
4. Create a folder named _iaqwif_" in the Libraries folder, then copy the _IaqWifi.h_ from src folder, paste it into the new folder, and rename it to _iaqwifi.h_
5. Open Communication Unit\iaqazure\src\azure\iot_configs.h change the wifi and Microsoft Azure settings.
6. Open Communication Unit\iaqazure\iaqazure.ino and update the ID to match the ID in the sensor unit code.
7. Upload the Communication unit code to the ESP-32 **S2**.
#### Sensor Unit Code
1. Install the [libraries](#libraries) for the sensor unit.
2. Open the file iaq_sensor located in Sensor Unit\IAQ_Sensor_Unit_Code.
3. Make sure that _BLE_MODE_ = 2 and that each sensor has an unique _SENSOR_ID_.
4. Upload the Sensor unit code to the ESP-32 **S3**.
#### Mobile App
1. Import the project into [MIT App Inventor](https://ai2.appinventor.mit.edu/).
2. Export the apk or use the [MIT Companion App](https://play.google.com/store/apps/details?id=edu.mit.appinventor.aicompanion3) to open the app.
3. Allow any permission requests by the app.

More details on how to set up are available in this [post](https://www.hackster.io/Diterragames/indoor-air-quality-sensors-iaq-27ae14#overview) in hackster.io.

## Libraries
#### Sensor Unit Libraries
- SPI
- Wire
- Adafruit_Sensor
- Adafruit_GFX
- Adafruit_ST7789
- Adafruit_SCD30
- Adafruit_PM25AQI
- BME68x Library
- [Bosch BSEC 2](https://github.com/boschsensortec/Bosch-BSEC2-Library/tree/master)
- Hardware Serial
- BLE Device
- BLE Advertising
- Any dependencies for these libraries
#### Communication Unit Libraries
- Azure SDK for C	
- ArduinoJson  
- Any dependencies for these libraries

## Thanks
I want to thank Dr. Simon Li who sponsored us by providing the needed components and accompanied the entire progress of this project. Thanks to my partner, Dr. Sheng Li, for participating in this project and assisting me. Last but not least the University of Calgary Electrical Makerspace for supplying us with materials, and allowing us to use its spaces and equipment to bring this project to reality.


## Contact
diogo.goto@gmail.com
