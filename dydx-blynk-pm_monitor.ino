/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************
  This example runs directly on NodeMCU.

  Note: This requires ESP8266 support package:
    https://github.com/esp8266/Arduino

  Please be sure to select the right NodeMCU module
  in the Tools -> Board menu!

  For advanced settings please follow ESP examples :
   - ESP8266_Standalone_Manual_IP.ino
   - ESP8266_Standalone_SmartConfig.ino
   - ESP8266_Standalone_SSL.ino

  Change WiFi ssid, pass, and Blynk auth token to run :)
  Feel free to apply it to any other example. It's simple!
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial
//#define fanPin 12
#define wifiResetBtn D5
#define dhtPin D6
#include "DHT.h"
#define DHTTYPE DHT11
#include <Wire.h> 
#include <SoftwareSerial.h>
SoftwareSerial mySerial(D3,D4); // RX, TX
DHT dht(dhtPin, DHTTYPE);
unsigned int pm1 = 0;
unsigned int pm2_5 = 0;
unsigned int pm10 = 0;
bool isInputArrFull = false;
int input[10];

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <WiFiManager.h>
WiFiManager wm;
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "s__ZSW4Iu7TMILQAbvNEqrqZ-SkJPds3";
//char auth[] = "vB8IO3nZpycpEY3yrUfSIGZBxTQ78pKt";

// Your WiFi credentials.
// Set password to "" for open networks.
//char ssid[] = ".Pvpeez13";
//char pass[] = "0912010056";  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);
void setup()
{
  // Debug console
  Serial.begin(9600);

  WiFiManager wm;
  pinMode(wifiResetBtn,INPUT);

  int btnState = digitalRead(wifiResetBtn);
    if(btnState == 0){
      wm.resetSettings();
    }
  
  //pinMode(fanPin,OUTPUT);
  //digitalWrite(fanPin,HIGH);
  //reset settings - wipe credentials for testing
    //wm.resetSettings();
     
    // Automatically connect using saved credentials,
    // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
    // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
    // then goes into a blocking loop awaiting configuration and will return success result

    bool res;
    // res = wm.autoConnect(); // auto generated AP name from chipid
     res = wm.autoConnect("Dy|Dx Air AP"); // anonymous ap
    //res = wm.autoConnect("AutoConnectAP","password"); // password protected ap
  
  Blynk.config(auth, IPAddress(185,78,164,48), 8080);
  //Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);
  
  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

  Serial.println(F("DHTxx test!"));
  dht.begin();
  
  while (!Serial) ;
  mySerial.begin(9600);
}

void CalAQI(unsigned int pm2_5){
  if(pm2_5 <= 100){
    Blynk.virtualWrite(V1,round(pm2_5*1.67));
}
  else if(pm2_5 > 100 && pm2_5 <= 200){
    Blynk.virtualWrite(V1,round(pm2_5*2.22));
  }
  else if(pm2_5 > 200 && pm2_5 <= 300){
    Blynk.virtualWrite(V1,round(pm2_5*2.5));
  }
  else if(pm2_5 > 300 && pm2_5 <= 400){
    Blynk.virtualWrite(V1,round(pm2_5*1.6));
  }
  else if(pm2_5 > 400){
    Blynk.virtualWrite(V1,round(pm2_5*2));
  }
}
void FanOn(){
  digitalWrite(fanPin,LOW);
}
void FanOff(){
  digitalWrite(fanPin,HIGH);
}

void TempandHumu(){
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F "));
  Blynk.virtualWrite(V6,t);
  Blynk.virtualWrite(V7,f);
  Blynk.virtualWrite(V8,h);
}

void loop()
{
  Blynk.run();

  int btnState = digitalRead(D5);
  if(btnState == 0){
    setup();
  }
  
  int index = 0;
char value;
char previousValue;
 
while (mySerial.available()) {
value = mySerial.read();
if ((index == 0 && value != 0x42) || (index == 1 && value != 0x4d)){
Serial.println("Cannot find the data header.");
break;
}
 
if (index == 4 || index == 6 || index == 8 || index == 10 || index == 12 || index == 14) {
previousValue = value;
}
else if (index == 5) {
pm1 = 256 * previousValue + value;
Blynk.virtualWrite(V2,pm1);
Serial.print("{ ");
Serial.print("\"pm1\": ");
Serial.print(pm1);
Serial.print(" ug/m3");
Serial.print(", ");
}
else if (index == 7) {
pm2_5 = 256 * previousValue + value;
if(pm2_5 >= 30){
  FanOn();
}
else{
  FanOff();
}
Blynk.virtualWrite(V3,pm2_5);
CalAQI(pm2_5);
Serial.print("\"pm2_5\": ");
Serial.print(pm2_5);
Serial.print(" ug/m3");
Serial.print(", ");
}
else if (index == 9) {
pm10 = 256 * previousValue + value;
Blynk.virtualWrite(V4,pm10);
Serial.print("\"pm10\": ");
Serial.print(pm10);
Serial.print(" ug/m3");
} else if (index > 15) {
break;
}
index++;
}
while(mySerial.available()) mySerial.read();
Serial.println(" }");
delay(1000);
TempandHumu();
}
