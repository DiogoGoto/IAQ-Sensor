#include "iaq_sensor.h" 
std::string SENSOR_NAME = "IAQ_Sensor_2";
/* Includes all relevant libries
*  Makes all definitions
*  Declare all structures
*/


// Function Prototypes
//----------------------------------------------
void displayInit(); // Initializes Built-in Display
void SCD30Init(); // Initializes CO2 Sensor
void PMInit(); // Initializes PM Sensor
void BMEInit(); //Initializes VOC and Pressure Sensor
void prepWrite(uint8_t size, uint16_t color); // Sets cursos at 0,0 and configure color and font size
void intro(); //shows the intro to the sensor
void showTft(); // Print data on the display
void sendSerial();// Print data on Serial
void sendInterface(); //Send data to Dr. Sheng's Part through Serial
void sendBLE(); //Sends data to Mobile Devices
int getConnectedDevicesCount(); //return how many clients are connected
int getFreeClientSlot(); //Finds a slot for to connect a new device
int getClientSlotByConnId(uint16_t conn_id); //finds client by their connection ID
// ---------------------------------------------


//Modules Initializations
//----------------------------------------------
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST); // Display
Adafruit_SCD30  scd30; //CO2 Sensor
Adafruit_PM25AQI aqi = Adafruit_PM25AQI(); //PM Sensor
PM25_AQI_Data pmsa0031; //Data for the PM sensor
IaqBsec* iaqbme = new IaqBsec();// Pressure and VOC Sensor
HardwareSerial Interface(1); //Data Interface 
//----------------------------------------------

iaqData data_package; //Data collected by the sensors 
String message = ""; //String to send the IAQ data to the Communication unit
unsigned long  last_time = millis(); // Timer collect the data every minute

//BLE Variables
BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
DEVICE ble_devices[MAX_CONNECTIONS]; //initializes all variables to connect to multiple devices
/*
bool deviceConnected = false;
bool oldDeviceConnected = false;
std::string receivedString = "";
*/

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer, esp_ble_gatts_cb_param_t *param) {
      int idx = getFreeClientSlot();
      if (idx != -1) {
            ble_devices[idx].conn_id = param->connect.conn_id;
            ble_devices[idx].status = true;
            Serial.printf("Device connected, connection ID: %d\n", param->connect.conn_id);
        }
      // Continue advertising if not at max connections
        if (getConnectedDevicesCount() < MAX_CONNECTIONS) {
            pServer->startAdvertising();
            Serial.println("Continuing to advertise.");
        }
    };

    void onDisconnect(BLEServer* pServer, esp_ble_gatts_cb_param_t *param) {
        int idx = getClientSlotByConnId(param->disconnect.conn_id);
        if (idx != -1) {
            ble_devices[idx].conn_id = 0;
            ble_devices[idx].status = false;
            Serial.printf("Device disconnected, connection ID: %d\n", param->disconnect.conn_id);
        }
        if (getConnectedDevicesCount() < MAX_CONNECTIONS) {
            pServer->startAdvertising();
            Serial.println("Device disconnected, starting advertising.");
        }
        
    }

};
/*
// Callback class to handle characteristic events
class MyCharacteristicCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) {
        std::string value = pCharacteristic->getValue();
        if (value.length() > 0) {
            Serial.print("Received string: ");
            Serial.println(value.c_str());
            if (value == "disconnect") {
                disconnectClient();
            }
        }
    }
};
*/
// MAIN CODE
//========================================================================
void setup() {
  // Initilization sequence
  Serial.begin(115200);
  Serial.println("Time, Temp, RH, Pres, CO2, Gas, PM1, PM25, PM10");
  displayInit();
  intro();
  prepWrite(2,WHITE);
  SCD30Init();
  PMInit();
  BMEInit();
  delay(500);
  Interface.begin(9600, SERIAL_8N1, RXD2, TXD2);


  //BLE Setup
  BLEDevice::init(SENSOR_NAME); // Create the BLE Device
  pServer = BLEDevice::createServer(); // Create the BLE Server
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);// Create the BLE Service
  
  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID1,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );

  //pCharacteristic->setCallbacks(new MyCharacteristicCallbacks());
  pCharacteristic->addDescriptor(new BLE2902());// Create a BLE Descriptor
  pService->start(); // Start the service

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

void loop() {
  // Reads VOC Sensor
  iaqbme->read();

  if(millis()-last_time > SAMPLING_PERIOD-5000) //55 seconds
    digitalWrite(PMSA_SET, HIGH); //Enables the PM Sensor 5 seconds before measuring  

  if(millis()-last_time > SAMPLING_PERIOD){
    // Reads  CO2 Sensorts
    if (scd30.dataReady()){
      if (!scd30.read()){
        prepWrite(2,RED); 
        tft.println("Error reading CO2 data");
        delay(1000);
        return; 
      }
    }

    // Reads PM Sensor
    //delay(3000); // Delay for give proper time for the sensor motor start spinning
    if (! aqi.read(&pmsa0031)) {
      prepWrite(2,RED); 
      tft.println("Error reading PM data");
      delay(1000); 
      return;
    }
    digitalWrite(PMSA_SET, LOW); // Disables the Sensor

    // Collect the data from SCD 30
    data_package.temperature = scd30.temperature;
    data_package.humidity = scd30.relative_humidity;
    data_package.co2 = scd30.CO2;

    // Collect the data from PMSA 0031
    data_package.pm1dot0 = pmsa0031.pm10_env;
    data_package.pm2dot5 = pmsa0031.pm25_env;
    data_package.pm10 = pmsa0031.pm100_env;

    // Collect the data from BME 688
    data_package.gasTemperature = iaqbme->bme.temperature;
    data_package.gasHumidity= iaqbme->bme.humidity;
    data_package.gasPressure  = iaqbme->bme.pressure/100.0; //Convert to hPa
    data_package.gasResistance = iaqbme->bme.gas_resistance/1000.0; //converto to KOhms
    data_package.gasIaq = iaqbme->bme.iaq;
    data_package.gaseVoc = iaqbme->bme.evoc;
    data_package.gaseCo2 = iaqbme->bme.eco2;

    showTft();
    sendSerial();
    sendInterface();
    sendBLE();
    last_time = millis();
  }
  delay(500);
}
//========================================================================


// Function Definitions
//----------------------------------------------
void displayInit(){ // Initializes Built-in Display
  // turn on backlite
    pinMode(TFT_BACKLITE, OUTPUT);
    digitalWrite(TFT_BACKLITE, HIGH);

    // turn on the TFT / I2C power supply
    pinMode(TFT_I2C_POWER, OUTPUT);
    digitalWrite(TFT_I2C_POWER, HIGH);
    delay(10);

    // initialize TFT
    tft.init(135, 240); // Init ST7789 240x135
    tft.setRotation(3);
    tft.fillScreen(ST77XX_BLACK);
  // put your setup code here, to run once:
}

void SCD30Init(){ // Initializes CO2 Sensor
  // Try to initialize SCD30
  tft.println("Iniating CO2 Sensor");
  if (!scd30.begin()) {
    tft.setTextColor(RED);
    tft.print("Failed to find SCD30 chip");
    while (1) { delay(10); }
  }
  tft.setTextColor(GREEN);
  tft.println("SCD30 Found!");
  
  scd30.setMeasurementInterval(60);
  tft.setTextColor(WHITE);
  tft.print("Interval: "); 
  tft.print(scd30.getMeasurementInterval()); 
  tft.println(" S");
}

void PMInit(){ // Initializes PM Sensor
  pinMode(PMSA_SET, OUTPUT);
  digitalWrite(PMSA_SET, HIGH); 
  tft.println("Iniating PM Sensor");
  delay(3000); //Wait for Sensor to boot up
    if (! aqi.begin_I2C()) {      // connect to the sensor over I2C
    tft.setTextColor(RED);
    tft.println("Could not find PM 2.5 sensor!");
    while (1) delay(10);
  }
  tft.setTextColor(GREEN);
  tft.println("PM25 found!");
  digitalWrite(PMSA_SET, LOW); 
}

void BMEInit(){ //Initializes VOC and Pressure Sensor
  tft.setTextColor(WHITE);
  tft.println(F("Iniating VOC Sensor"));

  iaqbme->setup();

  tft.setTextColor(GREEN);
  tft.println("BME680 found!");
}

void prepWrite(uint8_t size, uint16_t color){ // Sets cursos at 0,0 and configure color and font size
  tft.fillScreen(BLACK);
  tft.setTextSize(size);
  tft.setCursor(0, 0);
  tft.setTextColor(color);
}

void intro(){ //shows the intro to the sensor
  tft.fillScreen(WHITE);
  tft.setCursor(0, tft.height()/2 -30);
  tft.setTextSize(4);
  tft.setTextColor(BLUE);
  tft.println("IAQ Sensor");

  tft.setCursor(0, tft.height()-35);
  tft.setTextSize(2);
  tft.setTextColor(BLACK);
  tft.println("By: Simon Li, Sheng Li, Diogo Goto");
  delay(500);

}

void showTft(){ // Print data on the display
  prepWrite(2,WHITE);
  tft.print("Temp  : ");  tft.print(data_package.gasTemperature); tft.println(" *C");
  tft.print("RH    : ");  tft.print(data_package.humidity);    tft.println(" %");
  tft.print("Pres  : ");  tft.print(iaqbme->bme.pressure/100.0); tft.println(" hPa");
  tft.print("CO2   : ");  tft.print(data_package.co2);         tft.println(" ppm");
  tft.print("eVOC  : ");  tft.print(data_package.gaseVoc);     tft.println(" PPM");
  tft.print("PM 1.0: ");  tft.print(data_package.pm1dot0);     tft.println(" ug/m3");
  tft.print("PM 2.5: ");  tft.print(data_package.pm2dot5);     tft.println(" ug/m3");
  tft.print("PM 10 : ");  tft.print(data_package.pm10);        tft.println(" ug/m3");
}

void sendSerial(){// Print data on Serial
  Serial.print(millis());                    Serial.print(", ");
  Serial.print(data_package.gasTemperature);    Serial.print(", ");
  Serial.print(data_package.humidity);       Serial.print(", ");
  Serial.print(data_package.gasPressure);    Serial.print(", ");
  Serial.print(data_package.co2);            Serial.print(", ");
  Serial.print(data_package.gaseVoc);        Serial.print(", ");
  Serial.print(data_package.pm1dot0);        Serial.print(", ");
  Serial.print(data_package.pm2dot5);        Serial.print(", ");
  Serial.println(data_package.pm10);           //Serial.print("| ");
  /*
  Serial.print(data_package.gasTemperature); Serial.print(", ");
  Serial.print(data_package.gasHumidity);    Serial.print(", ");
  Serial.print(data_package.gasResistance);  Serial.print(", ");
  Serial.print(data_package.gaseVoc);        Serial.print(", ");
  Serial.print(data_package.gaseCo2);        Serial.print(", ");
  Serial.print(data_package.gasIaq);         Serial.print('\n');
*/
}

void sendInterface(){
  //T=26.24,RH=28.83,P=886.46,CO2=496.52,VOC=0.58,PM1.0=0.00,PM2.5=0.00,PM10=0.00,[\n]
   message = "T="                              +
             String(data_package.gasTemperature)  + ",RH=" +
             String(data_package.humidity)     + ",P=" +
             String(data_package.gasPressure)  + ",CO2=" +
             String(data_package.co2)          + ",VOC=" + 
             String(data_package.gaseVoc)      + ",PM1.0=" + 
             String(data_package.pm1dot0)      + ",PM2.5=" + 
             String(data_package.pm2dot5)      + ",PM10="+ 
             String(data_package.pm10)        + '\n';
  Interface.println(message);
}

void sendBLE(){
  // notify changed value
  for(int i =0; i< MAX_CONNECTIONS; i++){
    if (ble_devices[i].status) {
        pCharacteristic->setValue(message.c_str());
        pCharacteristic->notify();
        delay(3); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
    }
  }

}

int getConnectedDevicesCount() {
    int count = 0;
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (ble_devices[i].status) 
            count++;
    }
    return count;
}

int getFreeClientSlot() {
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (!ble_devices[i].status) {
            return i;
        }
    }
    return -1;
}

int getClientSlotByConnId(uint16_t conn_id) {
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (ble_devices[i].status && ble_devices[i].conn_id == conn_id) {
            return i;
        }
    }
    return -1;
}