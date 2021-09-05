#include <WiFi.h>
#include <ArduinoJson.h>
#include <binary.h>
#include <PubSubClient.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>


#define WIFI_STA_NAME "ANGKHANA" //change to your own ssid
#define WIFI_STA_PASS "61032522" //change to your own password

#define MQTT_SERVER "driver.cloudmqtt.com"
#define MQTT_PORT   18869
#define MQTT_USER_ID "tikproject"
#define MQTT_USERNAME "dothhsps"
#define MQTT_PASSWORD "TLjB0NeLDOrt"

#define RXD2 16
#define TXD2 17

char output[1024];
static const uint32_t GPSBaud = 9600;
double LAT;
double LONG;
String latitude;
String longitude;

WiFiClient client;
PubSubClient mqtt(client);
TinyGPSPlus gps;
HardwareSerial ss(2);

void setup()
{
    Serial.begin(9600);
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
    doc["lng"] = longitude;

    delay(100);

    serializeJson(doc, output);

    delay(1000);

    mqtt.publish("/locations", output);
}


void displayInfo()
{
    Serial.print(F("Location: "));
    if (gps.location.isValid())
    {
   LAT = gps.location.lat(),6;
   LONG = gps.location.lng(),6;

   latitude = String(LAT);
   longitude = String(LONG);

      Serial.print(gps.location.lat(),6);
      Serial.print(F(","));
      Serial.print(gps.location.lng(),6);
    }
   else
    {
      Serial.print(F("INVALID"));
    }

Serial.println();
}
