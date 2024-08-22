#ifndef _iaqnettime_
#define _iaqnettime_

#include "time.h"
#include <math.h>
#include <cstdlib>
#include <cmath>

const long  gmtOffset_sec = -25200; 
const int   daylightOffset_sec = 3600; /*1 hour daylight offset*/
const char* ntpServer = "pool.ntp.org";

class IaqNettime{
public:
    struct tm timeinfo;
    bool isok=false;
    char timestr[25];

    bool setup()
    {
      if(!isok)
      {
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
        printLocalTime();
      }
      return isok;
    }

    bool _getLocalTime()
    {
      isok = getLocalTime(&timeinfo);
      if(!isok)
      {
          Serial.println("Failed to obtain time");
      }
      return isok;
    }

  char* printLocalTimeStr() 
      {
          if(!_getLocalTime()) return NULL;
          // struct tm timeinfo;
          // if(!getLocalTime(&timeinfo)){
          //     Serial.println("Failed to obtain time");
          //     return -1;
          // }
          //char timestampStr[20];
          strftime(timestr, sizeof(timestr), "%Y-%m-%dT%H:%M:%S", &timeinfo);
          //strftime(timestr, sizeof(timestr), "%Y-%m-%dT%H:%M:%S%z", &timeinfo);
          Serial.println(timestr);
          //return atoll(timestampStr);
          return timestr;
      }
    long long int printLocalTime() 
    {
        if(!_getLocalTime()) return -1;
        // struct tm timeinfo;
        // if(!getLocalTime(&timeinfo)){
        //     Serial.println("Failed to obtain time");
        //     return -1;
        // }
        char timestampStr[20];
        strftime(timestampStr, sizeof(timestampStr), "%Y-%m-%dT%H:%M:%S", &timeinfo);
        Serial.println(timestampStr);
        return atoll(timestampStr);
    }

    long long int printLocalDate() 
    {
        if(!_getLocalTime()) return -1;
        // struct tm timeinfo;
        // if(!getLocalTime(&timeinfo)){
        //     Serial.println("Failed to obtain time");
        //     return -1;
        // }
        char dateStr[20];
        strftime(dateStr, sizeof(dateStr), "%Y-%m-%d %H:%M", &timeinfo);
        Serial.println(dateStr);
        return atoll(dateStr);
    }
};


#endif