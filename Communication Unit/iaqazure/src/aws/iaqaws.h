#ifndef _iaqaws_h
#define _iaqaws_h

#include "Secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"

#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"


void messageHandler(char* topic, byte* payload, unsigned int length)
{
  Serial.print("incoming: ");
  Serial.println(topic);

  // Handle incoming messages if needed
}

class IaqAws
{
public:
    WiFiClientSecure* net;// = WiFiClientSecure();
    PubSubClient* client;//(net);

    int trytimes=5;
    bool iswificonnected=false;
    bool isconnected=false;

    bool setup()
    {
      net = new WiFiClientSecure();
      client = new PubSubClient(*net);

      return connectAWS();
    }

    bool connectWifi()
    {
        iswificonnected = (WiFi.status() == WL_CONNECTED);

        if(!iswificonnected)
        {
            WiFi.mode(WIFI_STA);
            WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

            Serial.println("Connecting to Wi-Fi");

            for(int i=0; i<trytimes; i++)
            {
                iswificonnected = (WiFi.status() == WL_CONNECTED);
                if (!iswificonnected)
                {
                    delay(100);
                    Serial.print(".");
                }
                else{
                    Serial.println("Wi-Fi connected!");
                    break;
                }
            }
        }
        
        return iswificonnected;
    }

    bool isConnected()
    {
        Serial.print("AWS Iot connection: ");
        Serial.println(client->connected());
        return isconnected;
    }

    bool connectAWS()
    {
        if(!connectWifi()) return false;

        isconnected = client->connected();
        //Serial.println(client->connected());
        //Serial.println(isconnected);
        if(!isconnected)
        {
            // Configure WiFiClientSecure to use the AWS IoT device credentials
            net->setCACert(AWS_CERT_CA);
            net->setCertificate(AWS_CERT_CRT);
            net->setPrivateKey(AWS_CERT_PRIVATE);

            // Connect to the MQTT broker on the AWS endpoint we defined earlier
            client->setServer(AWS_IOT_ENDPOINT, 8883);
            client->setKeepAlive(300);

            // Create a message handler
            client->setCallback(messageHandler);

            Serial.println("Connecting to AWS IoT");

            for(int i=0; i<trytimes; i++)
            {
                if (!client->connect(THINGNAME))
                {
                    Serial.print(".");
                    delay(100);
                }
                else
                    break;
            }

            isconnected = client->connected();
            if (!isconnected)
            {
                Serial.println("AWS IoT Timeout!");
            }
            else
            {
                // Subscribe to a topic
                client->subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
                Serial.println("AWS IoT Connected!");
            }
        }
        //Serial.println(client->connected());
        //Serial.println(isconnected);
        return isconnected;
    }

    void publishMessage(float sensorid, float temperature, float humidity, float pressure, 
        float gasResistance, float co2conc, float pm2dot5, long long int timestamp, long long int date)
    {
        if(!isconnected) return;

        StaticJsonDocument<200> doc;
        doc["datetime"] = timestamp; // Add timestamp (in milliseconds)
        doc["datetime_sort"] = date;
        doc["sensor_id"] = sensorid;
        doc["temp"] = temperature;
        doc["humidity"] = humidity;
        doc["pressure"] = pressure;
        doc["voc_conc"] = gasResistance;
        doc["co2_conc"] = co2conc;
        doc["pm25"] = pm2dot5;
        char jsonBuffer[512];
        serializeJson(doc, jsonBuffer);

        client->publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
        //Serial.println(client->connected());
        client->loop();
        //Serial.println(client->connected());
    }
};


#endif

