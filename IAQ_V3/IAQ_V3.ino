#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include "Adafruit_ST7789.h" // Hardware-specific library for ST7789
#include <Adafruit_SCD30.h>  // CO2 Sensor
#include <Adafruit_PM25AQI.h> // PM Sensor
#include "Adafruit_BME680.h" // Pressure and VOC Sensor


#define PMSA_SET 13

#define SEALEVELPRESSURE_HPA (1013.25) // Constant 

// Colors definitions
// Colors are represented by 16bits values
//----------------------------------------------
#define BLACK 0x0000
#define WHITE 0xFFFF
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define ORANGE 0xFC00
//----------------------------------------------


// Function Prototypes
//----------------------------------------------
void displayInit(); // Initializes Built-in Display
void SCD30Init(); // Initializes CO2 Sensor
void PMInit(); // Initializes PM Sensor
void BMEInit(); //Initializes VOC and Pressure Sensor
void prepWrite(uint8_t size, uint16_t color); // Sets cursos at 0,0 and configure color and font size
void intro(); //shows the intro to the sensor
// ---------------------------------------------


//Modules Initializations
//----------------------------------------------
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST); // Display
Adafruit_SCD30  scd30; //CO2 Sensor
Adafruit_PM25AQI aqi = Adafruit_PM25AQI(); //PM Sensor
PM25_AQI_Data pmsa0031; //Data for the PM sensor
Adafruit_BME680 bme; // Pressure and VOC Sensor
unsigned long endTime = 0; // BME measurment end time
//----------------------------------------------



// MAIN CODE
//========================================================================
void setup() {
  // Initilazion sequence
  displayInit();
  intro();
  prepWrite(2,WHITE);
  SCD30Init();
  PMInit();
  BMEInit();
  delay(2000);
  Serial.begin(115200);
  Serial.println("Time, Temp, RH, Pres, CO2, Gas, PM1, PM25, PM10");
}

void loop() {
    digitalWrite(PMSA_SET, HIGH); // Enables the PM Sensor
  // Reads VOC Sensor
  endTime = bme.beginReading();
  if (endTime == 0) {
    prepWrite(2,RED); 
    tft.println(F("Error start reading VOC"));
    delay(1000);
    return;
  }

  

  // Reads  CO2 Sensorts
  if (scd30.dataReady()){
    if (!scd30.read()){
      prepWrite(2,RED); 
      tft.println("Error reading CO2 data");
      delay(1000);
      return; 
    }
  }
  if (!bme.endReading()) {
    prepWrite(2,RED); 
    tft.println(F("Failed to complete reading :("));
    delay(1000);
    return;
  }

  // Reads PM Sensor
  delay(2700); 
  if (! aqi.read(&pmsa0031)) {
    prepWrite(2,RED); 
    tft.println("Error reading PM data");
    delay(1000); 
    return;
  }
  digitalWrite(PMSA_SET, LOW); // Disables the Sensor


    // Print data on the display
    prepWrite(2,WHITE);
    tft.print("Temp: "); tft.print(scd30.temperature); tft.println(" C");
    tft.print("RH: "); tft.print(scd30.relative_humidity); tft.println(" %");
    tft.print("Pressure: "); tft.print(bme.pressure / 100.0); tft.println(" hPa");
    tft.print("CO2: "); tft.print(scd30.CO2, 3); tft.println(" ppm");
    tft.print("Gas: "); tft.print(bme.gas_resistance / 1000.0); tft.println(" KOhms");
    tft.print(F("PM 1.0: ")); tft.print(pmsa0031.pm10_env); tft.println(" ug/m3");
    tft.print(F("PM 2.5: ")); tft.print(pmsa0031.pm25_env); tft.println(" ug/m3");
    tft.print(F("PM 10 : ")); tft.print(pmsa0031.pm100_env); tft.println(" ug/m3");

    // Print data on Serial
    Serial.print(millis()); Serial.print(", ");
    Serial.print(scd30.temperature); Serial.print(", ");
    Serial.print(scd30.relative_humidity); Serial.print(", ");
    Serial.print(bme.pressure / 100.0); Serial.print(", ");
    Serial.print(scd30.CO2); Serial.print(", ");
    Serial.print(bme.gas_resistance / 1000.0); Serial.print(", ");
    Serial.print(pmsa0031.pm10_env); Serial.print(", ");
    Serial.print(pmsa0031.pm25_env); Serial.print(", ");
    Serial.println(pmsa0031.pm100_env);

  delay(5000);
}
//========================================================================


// Function Definitions
//----------------------------------------------
void displayInit(){
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

void SCD30Init(){
  // Try to initialize SCD30
  tft.println("Iniating CO2 Sensor");
  if (!scd30.begin()) {
    tft.setTextColor(RED);
    tft.print("Failed to find SCD30 chip");
    while (1) { delay(10); }
  }
  tft.setTextColor(GREEN);
  tft.println("SCD30 Found!");
  
  scd30.setMeasurementInterval(10);
  tft.setTextColor(WHITE);
  tft.print("Interval: "); 
  tft.print(scd30.getMeasurementInterval()); 
  tft.println(" S");
}

void PMInit(){
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

void BMEInit(){
  tft.setTextColor(WHITE);
  tft.println(F("Iniating VOC Sensor"));

  if (!bme.begin()) {
    tft.setTextColor(RED);
    tft.println("Could not find BME680!");
    while (1);
  }

  tft.setTextColor(GREEN);
  tft.println("BME680 found!");

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
}

void prepWrite(uint8_t size, uint16_t color){
  tft.fillScreen(BLACK);
  tft.setTextSize(size);
  tft.setCursor(0, 0);
  tft.setTextColor(color);
}

void intro(){
  tft.fillScreen(WHITE);
  tft.setCursor(0, tft.height()/2 -30);
  tft.setTextSize(4);
  tft.setTextColor(BLUE);
  tft.println("IAQ Sensor");

  tft.setCursor(0, tft.height()-35);
  tft.setTextSize(2);
  tft.setTextColor(BLACK);
  tft.println("By: Simon Li, Sheng Li, Diogo Goto");
  delay(2000);

}