#include <WiFi.h>
#include <ArduinoJson.h>
#include <binary.h>
#include <PubSubClient.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>


#define WIFI_STA_NAME "ANGKHANA" //change to your own ssid
#define WIFI_STA_PASS "61032522" //change to your own password

#define MQTT_SERVER "m16.cloudmqtt.com"
#define MQTT_PORT 16319
#define MQTT_USER_ID "GESTURE01"
#define MQTT_USERNAME "ggaomyqh"
#define MQTT_PASSWORD "3wjA27NFU3ET"

#define RXD2 16
#define TXD2 17

char output[1024];
static const uint32_t GPSBaud = 9600;

WiFiClient client;
PubSubClient mqtt(client);
TinyGPSPlus gps;
HardwareSerial ss(2);

void setup()
{
    Serial.begin(19200);
    ss.begin(GPSBaud, SERIAL_8N1, RXD2, TXD2, false);
    Serial.println(TinyGPSPlus::libraryVersion());
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(WIFI_STA_NAME);

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_STA_NAME, WIFI_STA_PASS);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }

    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    mqtt.setServer(MQTT_SERVER, MQTT_PORT);
}

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
      while (ss.available() > 0)
      if (gps.encode(ss.read()))
      displayInfo();

      if (millis() > 5000 && gps.charsProcessed() < 10)
      {
       Serial.println(F("No GPS detected: check wiring."));
       while(true);
      }
    
   
       sendDataToServer();
    }
}

void sendDataToServer()

{
    long prob = 0;
    StaticJsonDocument<1024> doc;

    doc["type"] = "ferry";
    doc["ship_id"] = "tik"; //add docID same with firebase per each ship
    doc["lat"] = latitude;
    doc["lng"] = longitude,;

    delay(100);

    serializeJson(doc, output);

    delay(100);

    mqtt.publish("/locations", output);
}

void displayInfo()
{
    Serial.print(F("Location: "));
    if (gps.location.isValid())
    {
      double latitude = (gps.location.lat(), 6);
      double longitude = (gps.location.lng(), 6);

      Serial.print(gps.location.lat(), 6);
      Serial.print(F(","));
      Serial.print(gps.location.lng(), 6);
    }
   else
    {
      Serial.print(F("INVALID"));
    }

Serial.println();
}
