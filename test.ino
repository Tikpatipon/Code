#include "TEE_UC20.h"
#include "SoftwareSerial.h"
#include <AltSoftSerial.h>
#include "internet.h"
#include <ArduinoJson.h>
#include <binary.h>
#include <PubSubClient.h>
#include "gnss.h" 
GNSS gps; 

INTERNET net;

//SIM TRUE  internet
#define APN "internet"
#define USER ""
#define PASS ""

#define MQTT_SERVER "m16.cloudmqtt.com"
#define MQTT_PORT "16319"
#define MQTT_USER_ID "GESTURE01"
#define MQTT_USERNAME "ggaomyqh"
#define MQTT_PASSWORD "3wjA27NFU3ET"

char output[1024];

AltSoftSerial mySerial;

void debug(String data)
{
  Serial.println(data);
}
void setup() 
{
  Serial.begin(9600);
  gsm.begin(&mySerial,9600);
  gsm.Event_debug = debug;
  Serial.println(F("UC20"));
  gsm.PowerOn(); 
  while(gsm.WaitReady()){}
  Serial.print(F("GetOperator --> "));
  Serial.println(gsm.GetOperator());
  Serial.print(F("SignalQuality --> "));
  Serial.println(gsm.SignalQuality());
  
  Serial.println(F("Disconnect net"));
  net.DisConnect();
  Serial.println(F("Set APN and Password"));
  net.Configure(APN,USER,PASS);
  Serial.println(F("Connect net"));
  net.Connect();
  Serial.println(F("Show My IP"));
  Serial.println(net.GetIP());
  mqtt.setServer(MQTT_SERVER, MQTT_PORT);
}

String readString; 
int ind1 = 0, ind2 = 0, ind3 = 0, ind4 = 0; 
String latitude = "0" , longitude = "0" , timeStr = ""; 

void loop() 
{
  if (mqtt.connected() == false)
    {
        Serial.print("MQTT connection... ");
        if (mqtt.connect(MQTT_USER_ID, MQTT_USERNAME, MQTT_PASSWORD))
        {
            Serial.println("connected");
        }
        else
        {
            Serial.println("failed");
            delay(5000);
        }
    }
    else
    {
        mqtt.loop();

       readString = gps.GetPosition(); 
  Serial.println(readString); 
  ind1 = readString.indexOf(':'); 
  ind2 = readString.indexOf(',', ind1 + 1 ); 
  timeStr = readString.substring(ind1 + 1, ind2); 
  ind3 = readString.indexOf(',', ind2 + 1 ); 
  latitude = readString.substring(ind2 + 1, ind3); 
  ind4 = readString.indexOf(',', ind3 + 1 ); 
  longitude = readString.substring(ind3 + 1, ind4); 
  Serial.println(latitude); 
  Serial.println(longitude);  
  sendDataToServer();
    }
}
void sendDataToServer()
{
    long prob = 0;
    StaticJsonDocument<1024> doc;

    doc["type"] = "ferry";
    doc["ship_id"] = "Bsirp1xitcgrLq17EHvC"; //add docID same with firebase per each ship
    doc["lat"] = 0.0;
    doc["lng"] = 0.0;

    serializeJson(doc, output);
    delay(100);
    
    mqtt.publish("/locations",output);
}
