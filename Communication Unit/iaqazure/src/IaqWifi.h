#ifndef _iaqwifi_h
#define _iaqwifi_h
#include <WiFi.h>

class IaqWifi
{
public:

  char* ssid=NULL;
  char* password=NULL;

  bool isconnected()
  {
    return (WiFi.status() == WL_CONNECTED);
  }

  bool connectToWiFi()
  {
    if(!isconnected())
    {
      Serial.println("Connecting to WIFI SSID " + String(ssid));

      WiFi.mode(WIFI_STA);
      WiFi.disconnect();
      delay(100);
      WiFi.begin(ssid);
      delay(500);
      // while (WiFi.status() != WL_CONNECTED)
      // {
      //   delay(500);
      //   Serial.print(".");
      // }
      if(isconnected())
      {
        Serial.println("... connected, ip= "+WiFi.localIP().toString());
      }
      else
      {
        Serial.println("... failed.");
      }
    }
    return isconnected();
  }


};



#endif