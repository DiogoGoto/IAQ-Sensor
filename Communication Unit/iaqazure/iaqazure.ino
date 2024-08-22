#define PART_IOT 1 // 0=sensor; 1=iot; Leave this line as is
#define PART_ID 1 // change here for each device
#define IAQ_AZURE_IOT_ID PART_ID 

#include <Wire.h>
#include <SPI.h>

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// --- network;
#include "src/IaqWifi.h"
//#include "iaqaws.h"
#include "src/azure/azure.h"
#include "src/iaqnettime.h"

IaqWifi* iaqwifi = new IaqWifi();
IaqNettime iaqnettime = IaqNettime();
//IaqAws iaqaws = IaqAws();
Azure* iaqiot= new Azure();
#include <ArduinoJson.h>

// --- sensor modules;
#if PART_IOT ==1
#include <Adafruit_Sensor.h>
#include <IaqCo2.h>
//#include <IaqBme680.h>
//#include <iaqbme68x.h>
#include "src/iaqbsec.h"  // BME688
//#include <iaqSGP40.h>
#include <IaqPm.h>

IaqCo2 *iaqco2 = new IaqCo2();
IaqPm *iaqpm = new IaqPm();
IaqBsec* iaqbme = new IaqBsec();
//IaqBme68x* iaqbme = new IaqBme68x();
//IaqBme680 *iaqbme = new IaqBme680();
//IaqSGP40* iaqsgp = new IaqSGP40();
#endif

struct Iaqdata
{
  float sensorid = PART_ID;
  float temperature = 0.0;
  float humidity = 0.0;
  float pressure = 0.0;
  float gasResistance = 0.0;
  float gasiaq=0.0;
  float gasevoc=0.0;
  float gaseco2=0.0;
  float co2conc = 0.0;
  float pm2dot5 = 0.0;
  float pm1_0 = 0.0;
  float pm10 = 0.0;

  long long int timestamp = 0;
  long long int date = 0;

}iaqdata;
char jsonBuffer[512];

// --- other variables;
unsigned long last_time = millis();


void setup()
{
  delay(100);
  Serial.begin(115200);
  Serial1.begin(9600);
  Interface.begin(9600, SERIAL_8N1, RXD2, TXD2);
  delay(5000);

  Serial.println("--- iaq azure setup---");
  //iaqaws.setup();
  iaqiot->iaqwifi = iaqwifi;
  iaqiot->setup();
  iaqnettime.setup();

  setuptft();

#if PART_IOT==0
  iaqco2->setup();
  iaqpm->setup();
  iaqbme->setup();
  //bme_setup();
  //iaqsgp->setup();
#endif

}

bool isok=false;
int delaytime=300;
int iottime=50*1000/delaytime; // *100ms;
int iotcnt=iottime;   // for send data to iot server;
int readtime=3*1000/delaytime; // 2s;
int readcnt=readtime;
//int cnt=iotcnt-2000/delaytime;

unsigned char msgid=0;
bool isdata = false;

void loop() 
{
  isok = false;

  // if(iotcnt++>=iottime)
  // {
  //   iotcnt=0;
  //   //iaqaws.connectAWS();
  //   iaqwifi->connectToWiFi();
  //   //iaqiot->loop();
    
  //   if(iaqnettime.setup()){
  //     iaqdata.timestamp = iaqnettime.printLocalTime(); // Get the current timestamp
  //     iaqdata.date = iaqnettime.printLocalDate();
  //   }
  // }

#if PART_IOT==0
  iaqbme->read();

  if(readcnt++ >= readtime)
  {
    readcnt = 0;

    if(!iaqco2->read()) isok = false;
    iaqpm->read();
    //bme_read();

    //temperature = round((iaqco2->scd30.temperature -3.5)* 100) / 100;
    iaqdata.temperature = iaqbme->bme.temperature;
    iaqdata.humidity = iaqbme->bme.humidity;//round(iaqbme->bme.humidity * 100) / 100;  // %
    iaqdata.pressure = iaqbme->bme.pressure/100.0;//round((iaqbme->bme.pressure / 100.0) * 100) / 100; // Convert pressure to hPa
    iaqdata.gasResistance = iaqbme->bme.gas_resistance/1000.0;//round((iaqbme->bme.gas_resistance / 1000.0) * 100) / 100; // Convert gas resistance to kOhms
    iaqdata.gaseco2 = iaqbme->bme.eco2;
    iaqdata.gasevoc = iaqbme->bme.evoc;
    iaqdata.gasiaq = iaqbme->bme.iaq;

    iaqdata.co2conc = round(iaqco2->scd30.CO2 * 100) / 100;
    iaqdata.pm2dot5 = iaqpm->data.pm25_standard;
    iaqdata.pm1_0 = iaqpm->data.pm10_standard;
    iaqdata.pm10 = iaqpm->data.pm100_standard;

    sendSerial(&Serial);
    sendSerial(&Serial1);

    tft.fillScreen(ST77XX_BLACK); // Clear the TFT screen
    tft.setCursor(0, 0);
    tft.setTextSize(2);
    tft.setTextColor(ST77XX_GREEN);
    showTft();

    isdata = true;
  }
#else   // iot part;
  // receive data from serial port 1;
  if(receiveSensor(&Serial1)||receiveSensor(&Interface)) 
  {
    isdata = true;

    //sendSerial(&Serial);

    tft.fillScreen(ST77XX_BLACK); // Clear the TFT screen
    tft.setCursor(0, 0);
    tft.setTextSize(2);
    tft.setTextColor(ST77XX_GREEN);
    showTft();
    last_time = millis();
  }
  if(millis() - last_time > 65000){//Does not receive a message in the last 65 seconds
    tft.fillScreen(ST77XX_BLACK); // Clear the TFT screen
    tft.setCursor(0, 0);
    tft.setTextSize(3);
    tft.setTextColor(ST77XX_RED);
    tft.println("Disconnected");
    last_time = millis();
    Serial.print(millis()); Serial.print(last_time); Serial.println(millis() - last_time);
  }
#endif

  if(iotcnt++>=iottime && isdata)
  {
    isdata = false;

    iotcnt=0;
    //iaqaws.connectAWS();
    iaqwifi->connectToWiFi();
    //iaqiot->loop();
    
    if(iaqnettime.setup()){
      iaqnettime.printLocalTimeStr();
      //iaqdata.timestamp = iaqnettime.printLocalTime(); // Get the current timestamp
      //iaqdata.date = iaqnettime.printLocalDate();
    }
    
    int d = setMsg(iaqdata.sensorid, iaqdata.temperature, iaqdata.humidity, iaqdata.pressure, 
       iaqdata.gasevoc,//iaqdata.gasResistance,
       iaqdata.co2conc,iaqdata.pm2dot5,
       msgid++,iaqnettime.timestr);
    iaqiot->loop(jsonBuffer, d);
  
    // iaqaws.publishMessage(iaqdata.sensorid, iaqdata.temperature, iaqdata.humidity, iaqdata.pressure, 
    //   iaqdata.gasResistance,iaqdata.co2conc,iaqdata.pm2dot5,
    //   iaqdata.timestamp,iaqdata.date);
  }

  delay(delaytime);
}


int setMsg(float sensorid, float temperature, float humidity, float pressure, 
        float gasResistance, float co2conc, float pm2dot5, unsigned char msg_id, const char* timestr)//long long int timestamp, long long int date)
{
    //if(!isconnected) return;

    StaticJsonDocument<200> doc;
    doc["id"] = sensorid;
    doc["msgid"] = msg_id; // Add timestamp (in milliseconds)
    doc["datetime"] = timestr;
    doc["T"] = temperature;
    doc["RH"] = humidity;
    doc["P"] = pressure;
    doc["eVOC"] = gasResistance;
    doc["CO2"] = co2conc;
    doc["PM25"] = pm2dot5;
    
    return serializeJson(doc, jsonBuffer);

    // client->publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
    // //Serial.println(client->connected());
    // client->loop();
    // //Serial.println(client->connected());
}
bool receiveSensor(Stream* sp)
{
  bool isall=false;
  String str;
  String s;
  int d;
  while(true)
  {
    if(sp->available())
    {
      str = sp->readStringUntil(','); // terminal char is discarded;
      try
      {
        d = str.indexOf('=');
        s = str.substring(0,d+1);
        str = str.substring(d+1); 
        float f = str.toFloat();
        //Serial.printf("%s=%.2f\n",str,f);

        if (s.equals("T="))          {iaqdata.temperature=f; Serial.printf("t=%.2f,", iaqdata.temperature);}
        else if (s.equals("RH="))    {iaqdata.humidity=f; Serial.printf("rh=%.2f,", iaqdata.humidity);}
        else if (s.equals("P="))     {iaqdata.pressure=f; Serial.printf("co2=%.2f,", iaqdata.pressure);}
        else if (s.equals("CO2="))   {iaqdata.co2conc=f; Serial.printf("co2=%.2f,", iaqdata.co2conc);}
        else if (s.equals("VOC="))   {iaqdata.gasevoc=f; Serial.printf("voc=%.2f,", iaqdata.gasevoc);}
        else if (s.equals("PM1.0=")) {iaqdata.pm1_0=f; Serial.printf("pm1.0=%.2f,", iaqdata.pm1_0);}
        else if (s.equals("PM2.5=")) {iaqdata.pm2dot5=f; Serial.printf("pm2.5=%.2f,", iaqdata.pm2dot5);}
        else if (s.equals("PM10="))  {iaqdata.pm10=f; Serial.printf("pm10=%.2f,\n", iaqdata.pm10); isall = true;}

      }
      catch(...)
      {
        Serial.println("convert error.");
      }
    }
    else
    {
      break;
    }
  }
  return isall;
}
void sendSerial(Stream* sp)
{
  sp->printf("T=%.2f,", iaqdata.temperature);
  sp->printf("RH=%.2f,", iaqdata.humidity);
  sp->printf("P=%.2f,", iaqdata.pressure);
  sp->printf("CO2=%.2f,", iaqdata.co2conc);
  sp->printf("VOC=%.2f,", iaqdata.gasevoc);
  sp->printf("PM1.0=%.2f,", iaqdata.pm1_0);
  sp->printf("PM2.5=%.2f,", iaqdata.pm2dot5);
  sp->printf("PM10=%.2f,", iaqdata.pm10);
  sp->println();
  // sp->print("T: ");  Serial.print(iaqdata.temperature); Serial.print(" °C");
  // Serial.print(", RH: ");  Serial.print(iaqdata.humidity); Serial.print(" %");
  // Serial.print(", P: ");  Serial.print(iaqdata.humidity); Serial.print(" hPa");
  // Serial.print(", IAQ: ");  Serial.print(iaqdata.gasiaq);
  // Serial.print(", eCO2: "); Serial.print(iaqdata.gaseco2);
  // Serial.print(", eVOC: "); Serial.print(iaqdata.gasevoc);
  // Serial.print(", R: "); Serial.print(iaqdata.gasResistance); Serial.println(" kOhms: ");
}

void showTft()
{
  tft.fillScreen(ST77XX_BLACK); // Clear the TFT screen
  tft.setCursor(0, 0);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_GREEN);

  //tft.print("T    : ");  tft.print(temperature - 3.5);  tft.println(" DegreeC");
  tft.print("T    : ");  tft.print(iaqdata.temperature);  tft.println(" *C");
  tft.print("P    : ");  tft.print(iaqdata.pressure);  tft.println(" hPa");
  tft.print("RH   : ");  tft.print(iaqdata.humidity);  tft.println("%");
  
  tft.print("PM2.5: ");  tft.print(iaqdata.pm2dot5);  tft.println("  units");
  tft.print("CO2  : ");  tft.print(iaqdata.co2conc);  tft.println("  ppm");
  tft.print("eCO2 : ");  tft.print(iaqdata.gaseco2);   tft.println("  ppm");
  tft.print("eVOC : ");  tft.print(iaqdata.gasevoc);  tft.println("   ppm");
  tft.print("IAQ  : ");  tft.println(iaqdata.gasiaq);

  // tft.print("SGP  : ");  tft.print(iaqsgp->raw/1000.0); tft.println(" kOhms");
  // tft.print("Index: ");  tft.println(gasiaq);
  //tft.print("VOC = "); tft.print(gasvoc);  tft.println(" ppm");
  //tft.print("Altitude = ");
  //tft.print(iaqbme->bme.readAltitude(SEALEVELPRESSURE_HPA));
}


void setuptft(void) {
  Serial.print(F("Hello! Feather TFT Test"));

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
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0,0);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_GREEN);
}
