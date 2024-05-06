#include <SPI.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include "Adafruit_ST7789.h" // Hardware-specific library for ST7789
#include <Adafruit_SCD30.h>  // CO2 Sensor
#include <Adafruit_PM25AQI.h> // PM Sensor


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
void prepWrite(uint8_t size, uint16_t color); // Sets cursos at 0,0 and configure color and font size
// ---------------------------------------------


//Modules Initializations
//----------------------------------------------
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST); // Display
Adafruit_SCD30  scd30; //CO2 Sensor
Adafruit_PM25AQI aqi = Adafruit_PM25AQI(); //PM Sensor
PM25_AQI_Data pmsa0031; //Data for the sensort
//----------------------------------------------



// MAIN CODE
//========================================================================
void setup() {
  displayInit();
  prepWrite(2,WHITE);
  SCD30Init();
  PMInit();



}

void loop() {
  // Reads  CO2 Sensorts
  if (scd30.dataReady())
    if (!scd30.read()){ tft.println("Error reading sensor data"); return; }
  
  // Reads PM Sensor
  if (! aqi.read(&pmsa0031)) {
    tft.println("Could not read from AQI");
    delay(500);  // try again in a bit!
    return;
  }

    // Print data
    prepWrite(2,WHITE);
    tft.print("Temp: "); tft.print(scd30.temperature); tft.println(" C");
    tft.print("RH: "); tft.print(scd30.relative_humidity); tft.println(" %");
    tft.print("CO2: "); tft.print(scd30.CO2, 3); tft.println(" ppm");

    tft.print(F("PM 1.0: ")); tft.print(pmsa0031.pm10_env); tft.println(" ug/m3");
    tft.print(F("PM 2.5: ")); tft.print(pmsa0031.pm25_env); tft.println(" ug/m3");
    tft.print(F("PM 10 : ")); tft.print(pmsa0031.pm100_env); tft.println(" ug/m3");
  delay(1000);
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

  tft.setTextColor(WHITE);
  tft.print("Interval: "); 
  tft.print(scd30.getMeasurementInterval()); 
  tft.println(" S");
}

void PMInit(){
  tft.println("Iniating PM Sensor");
  delay(3000); //Wait for Sensor to boot up
    if (! aqi.begin_I2C()) {      // connect to the sensor over I2C
    tft.setTextColor(RED);
    tft.println("Could not find PM 2.5 sensor!");
    while (1) delay(10);
  }
  tft.setTextColor(GREEN);
  tft.println("PM25 found!");
  tft.setTextColor(WHITE);
  delay(2000);

}


void prepWrite(uint8_t size, uint16_t color){
  tft.fillScreen(BLACK);
  tft.setTextSize(size);
  tft.setCursor(0, 0);
  tft.setTextColor(color);
}

