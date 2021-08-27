#include "TEE_UC20.h" 
#include "SoftwareSerial.h" 
#include <AltSoftSerial.h> 
#include "internet.h" 
#include "firebase.h" 
#include "gnss.h" 
GNSS gps; 
INTERNET net; 
FIREBASE firebase; 
//SIM internet 
#define APN "internet" 
#define USER "" 
#define PASS "" 
#define FIREBASE_URL      "ferrytracking-e7d8a-default-rtdb.asia-southeast1.firebasedatabase.app" 
#define FIREBASE_SECRET   "92yMCFowGyVDaoGcWoCclelqPBd4oHK67ACek4Jf" 
 
AltSoftSerial mySerial; 
void debug(String data) 
{ 
  Serial.println(data); 
} 
void data_out(char data) 
{ 
  Serial.write(data); 
} 
void setup() 
{ 
  Serial.begin(9600); 
  gsm.begin(&mySerial, 9600); 
  gsm.Event_debug = debug; 
  Serial.println(F("UC20")); 
  gsm.PowerOn(); 
  while (gsm.WaitReady()) {} 
  Serial.print(F("GetOperator --> ")); 
  Serial.println(gsm.GetOperator());  
 
  Serial.print(F("SignalQuality --> ")); 
  Serial.println(gsm.SignalQuality()); 
  Serial.println(F("Disconnect net")); 
  net.DisConnect(); 
  Serial.println(F("Set APN and Password")); 
  net.Configure(APN, USER, PASS); 
  Serial.println(F("Connect net")); 
  net.Connect(); 
  Serial.println(F("Show My IP")); 
  Serial.println(net.GetIP()); 
  Serial.print(F("GPS Start")); 
  gps.Start(); 
  firebase.begin(FIREBASE_URL, FIREBASE_SECRET); 
  int ret = firebase.connect(); 
  if (ret == 1) 
  { 
    Serial.println("FIREBASE CONNECT"); 
  } 
  firebase.close(); 
 
} 
String readString; 
int ind1 = 0, ind2 = 0, ind3 = 0, ind4 = 0; 
String latitude = "0" , longitude = "0" , timeStr = ""; 
void loop() 
{ 
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
 int ret = firebase.connect(); 
  if (ret == 1)  
 
  { 
    firebase.set("ferry1/lat1", latitude); 
    firebase.set("ferry1/long1", longitude);   
  } 
  firebase.close(); 
  delay(1000); 
} 
