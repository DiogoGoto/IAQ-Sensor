#include "iaq_sensor.h" 
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
String message = "";
unsigned long  last_time = millis();
unsigned long start_time = millis();
unsigned long timer_5s = millis();

// MAIN CODE
//========================================================================
void setup() {
  // Initilazion sequence
  displayInit();
  //intro();
  prepWrite(2,WHITE);
  SCD30Init();
  PMInit();
  BMEInit();
  delay(500);
  Serial.begin(115200);
  Interface.begin(9600, SERIAL_8N1, RXD2, TXD2);
  // Configure wake-up source to wake up every minute (60 seconds)
  esp_sleep_enable_timer_wakeup(4 * 60 * 1000000); // 4 minutes in microseconds
  start_time = millis();
  tft.print("Reading");
}

void loop() {
  // Reads VOC Sensor
  iaqbme->read();

  if(millis() - timer_5s > 5000){
    tft.print(".");
    timer_5s = millis();
  }

  if(millis()-last_time > 50000)
    digitalWrite(PMSA_SET,HIGH); // Turn on the PM sensor 10 seconds before the measurements time
  

  if(millis()-last_time > 60000){
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
    if (! aqi.read(&pmsa0031)) {
      prepWrite(2,RED); 
      tft.println("Error reading PM data");
      delay(1000); 
      return;
    }
    digitalWrite(PMSA_SET,LOW);
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
    delay(3000);
    last_time = millis();
  }
  if(millis()-start_time > 60000)
    esp_deep_sleep_start();
  

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
  
  scd30.setMeasurementInterval(25);
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
  tft.print("Temp  : ");  tft.print(data_package.temperature); tft.println(" *C");
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
  Serial.print(data_package.temperature);    Serial.print(", ");
  Serial.print(data_package.humidity);       Serial.print(", ");
  Serial.print(data_package.gasPressure);    Serial.print(", ");
  Serial.print(data_package.co2);            Serial.print(", ");
  Serial.print(data_package.gaseVoc);        Serial.print(", ");
  Serial.print(data_package.pm1dot0);        Serial.print(", ");
  Serial.print(data_package.pm2dot5);        Serial.print("| ");
  Serial.print(data_package.gasTemperature); Serial.print(", ");
  Serial.print(data_package.gasHumidity);    Serial.print(", ");
  Serial.print(data_package.gasResistance);  Serial.print(", ");
  Serial.print(data_package.gaseVoc);        Serial.print(", ");
  Serial.print(data_package.gaseCo2);        Serial.print(", ");
  Serial.print(data_package.gasIaq);         Serial.print('\n');

}

void sendInterface(){
  //T,RH,P,CO2,VOCs,PM1.0,PM2.5,PM10
   message = String(data_package.temperature) + "," +
            String(data_package.humidity)     + "," +
            String(data_package.gasPressure)  + "," +
            String(data_package.co2)          + "," + 
            String(data_package.gaseVoc)      + "," + 
            String(data_package.pm1dot0)      + "," + 
            String(data_package.pm2dot5)      + "," + 
            String(data_package.pm10);
  
  Interface.println(message);

}
