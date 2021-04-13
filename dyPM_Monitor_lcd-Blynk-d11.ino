/**************************************************************************
  This is a library for several Adafruit displays based on ST77* drivers.

  Works with the Adafruit 1.8" TFT Breakout w/SD card
    ----> http://www.adafruit.com/products/358
  The 1.8" TFT shield
    ----> https://www.adafruit.com/product/802
  The 1.44" TFT breakout
    ----> https://www.adafruit.com/product/2088
  The 1.14" TFT breakout
  ----> https://www.adafruit.com/product/4383
  The 1.3" TFT breakout
  ----> https://www.adafruit.com/product/4313
  The 1.54" TFT breakout
    ----> https://www.adafruit.com/product/3787
  The 2.0" TFT breakout
    ----> https://www.adafruit.com/product/4311
  as well as Adafruit raw 1.8" TFT display
    ----> http://www.adafruit.com/products/618

  Check out the links above for our tutorials and wiring diagrams.
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional).

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 **************************************************************************/

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

#if defined(ARDUINO_FEATHER_ESP32) // Feather Huzzah32
  #define TFT_CS         14
  #define TFT_RST        15
  #define TFT_DC         32

#elif defined(ESP8266)
  #define TFT_CS         D2
  #define TFT_RST        D3                                            
  #define TFT_DC         D4

#else
  // For the breakout board, you can use any 2 or 3 pins.
  // These pins will also work for the 1.8" TFT shield.
  #define TFT_CS        10
  #define TFT_RST        9 // Or set to -1 and connect to Arduino RESET pin
  #define TFT_DC         8
#endif

// OPTION 1 (recommended) is to use the HARDWARE SPI pins, which are unique
// to each board and not reassignable. For Arduino Uno: MOSI = pin 11 and
// SCLK = pin 13. This is the fastest mode of operation and is required if
// using the breakout board's microSD card.

// For 1.44" and 1.8" TFT with ST7735 use:
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// For 1.14", 1.3", 1.54", and 2.0" TFT with ST7789:
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);


// OPTION 2 lets you interface the display using ANY TWO or THREE PINS,
// tradeoff being that performance is not as fast as hardware SPI above.
//#define TFT_MOSI 11  // Data out
//#define TFT_SCLK 13  // Clock out

// For ST7735-based displays, we will use this call
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

// OR for the ST7789-based displays, we will use this call
//Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);


float p = 3.1415926;
#define BLYNK_PRINT Serial
#define wifiResetBtn D10
#define dhtPin D6
#include "DHT.h"
#define DHTTYPE DHT11
#include <Wire.h> 
#include <SoftwareSerial.h>
SoftwareSerial mySerial(D8,D9); // RX, TX
//SoftwareSerial ControlSerial(D11,D12); // RX, TX
DHT dht(dhtPin, DHTTYPE);
int pm1 = 0;
unsigned int pm2_5 = 0;
int pm10 = 0;
float t = 0;
float h = 0;
bool isDHTRead = false;
bool isPMRead = false;
bool isOff = false;
int currMode = 1;
int fanSpeed = 0;
unsigned int DisplayTime = 3000;
unsigned int lastDisplayTime = 0;
unsigned int reConnectTime = 0;
unsigned int PMTime = 2000;
unsigned int lastPM = 0;
unsigned int BtnTime = 50;
unsigned int lastBtn = 0;
unsigned int lastReconnect = 0;
//

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <WiFiManager.h>
WiFiManager wm;
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
//char auth[] = "s__ZSW4Iu7TMILQAbvNEqrqZ-SkJPds3"; //pm monitor
char auth[] = "Z07tA5yefNJnnRK84SjVLVhzjyh83YEf";
//char auth[] = "vB8IO3nZpycpEY3yrUfSIGZBxTQ78pKt";

// Your WiFi credentials.
// Set password to "" for open networks.
//char ssid[] = ".Pvpeez13";
//char pass[] = "0912010056";  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

#include <dydxIcon.h>

void setup() {
  
  Serial.begin(9600);
  Serial.print(F("Hello! ST77xx TFT Test"));
  manFanMode(0);
  // Use this initializer if using a 1.8" TFT screen:
  //tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab

  // OR use this initializer if using a 1.8" TFT screen with offset such as WaveShare:
  // tft.initR(INITR_GREENTAB);      // Init ST7735S chip, green tab

  // OR use this initializer (uncomment) if using a 1.44" TFT:
  //tft.initR(INITR_144GREENTAB); // Init ST7735R chip, green tab

  // OR use this initializer (uncomment) if using a 0.96" 160x80 TFT:
  //tft.initR(INITR_MINI160x80);  // Init ST7735S mini display

  // OR use this initializer (uncomment) if using a 1.3" or 1.54" 240x240 TFT:
  //tft.init(240, 240);           // Init ST7789 240x240

  // OR use this initializer (uncomment) if using a 2.0" 320x240 TFT:
  tft.init(240, 320);           // Init ST7789 320x240

  // OR use this initializer (uncomment) if using a 1.14" 240x135 TFT:
  //tft.init(135, 240);           // Init ST7789 240x135
  
  // SPI speed defaults to SPI_DEFAULT_FREQ defined in the library, you can override it here
  // Note that speed allowable depends on chip and quality of wiring, if you go too fast, you
  // may end up with a black screen some times, or all the time.
  tft.setSPISpeed(79999999);
  
  Serial.println(F("Initialized"));

  uint16_t time = millis();
  tft.fillScreen(ST77XX_BLACK);
  time = millis() - time;

  Serial.println(time, DEC);
  delay(500);

  tft.setRotation(3);
  // large block of text
  /*
  tft.fillScreen(ST77XX_BLACK);

  WelcomeSplash(ST77XX_WHITE);
  delay(3000);
  APSetup(ST77XX_WHITE);
  delay(3000);
  WifiConnected(".Pvpeez13");
  delay(3000);*/
  
  WelcomeSplash();
  WiFiManager wm;
  pinMode(wifiResetBtn,INPUT);

  int btnState = digitalRead(wifiResetBtn);
    if(btnState == 0){
      //wm.resetSettings();
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
    //res = wm.startConfigPortal("Dy|Dx Air AP"); // anonymous ap
    // res = wm.autoConnect(); // auto generated AP name from chipid
    Serial.println(WiFi.SSID());
     
    //res = wm.autoConnect("AutoConnectAP","password"); // password protected ap
  if(WiFi.SSID() == "" && WiFi.status()!= WL_CONNECTED){
      //ShowAPSetup();
      ShowQR_APSetup();
      res = wm.startConfigPortal("Dy|Dx Air AP");
          //delay(3000);
      if(res == false){
        ESP.reset();
        delay(5000);
      }

    }
  //Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);
  //if you get here you have connected to the WiFi
  delay(1000);
  if(WiFi.status() == WL_CONNECTED){
      WifiConnected(WiFi.SSID());
      WiFi.setAutoReconnect(true);
      Blynk.config(auth, IPAddress(185,78,164,48), 8080);
      Blynk.connect();
      Serial.println("connected...yeey :)");
      Blynk.virtualWrite(V4,1);
  }
  delay(1000);
  ShowAutoMode();
  delay(1000);
  //ShowNightMode();
  tft.fillRect(21,21,279,199,ST77XX_BLACK);
  Serial.println("done");
  //MainUI(pm2_5,t,h);
  
  while (!Serial) ;
  mySerial.begin(9600);
  //ControlSerial.begin(9600);
}
BLYNK_WRITE(4){
    if (param.asInt()){
      if(millis() - lastBtn > BtnTime){
        lastBtn = millis();
        if(isOff == false){
          isOff == true;
        }
        else if(isOff == true){
          //digitalWrite(rstBoard,HIGH);
          setup();
        }
      }

    } else {
        
    }
}
BLYNK_WRITE(6){
    if (param.asInt()){
      if(millis() - lastBtn > BtnTime){
        lastBtn = millis();
        ShowAutoMode();       
        currMode = 1;
      }

    } else {
        
    }
}
BLYNK_WRITE(7){
    if (param.asInt()){
      if(millis() - lastBtn > BtnTime){
        lastBtn = millis();
        ShowTurboMode();
        manFanMode(10);    
        currMode = 2;
      } 
    }
    else {
        
    }
}
BLYNK_WRITE(8){
    if (param.asInt()){
      if(millis() - lastBtn > BtnTime){
        lastBtn = millis();      
        ShowNightMode(); 
        manFanMode(0);
        currMode = 3;
      }
    } else {
        
    }
}

void loop() {
    PMMeasure();
    TempandHumu();
  if(millis() - lastDisplayTime > DisplayTime){
    lastDisplayTime = millis();
    
    MainUI(pm2_5,t,h);
  }
  //RefreshData();
  if(millis() - lastPM > PMTime){
    lastPM = millis();

    if(currMode == 1){
      pmToFanMode();
    }
  }
}

void WelcomeSplash(){
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(3, 3);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(true);
  tft.setTextSize(3);
  tft.print("\n\n\n");
  tft.setTextSize(1);
  tft.print("                ");
  tft.setTextSize(4);
  tft.println("Dy|Dx");
  tft.setTextSize(3);
  tft.println("\n   Air Purifier");
}
void ShowAPSetup(){
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(3, 3);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(true);
  tft.setTextSize(3);
  tft.print("\n\n");
  tft.setTextSize(2);
  tft.println("      Please connect");
  tft.setTextSize(4);
  tft.println("\n Dy|Dx Air AP\n");
  tft.setTextSize(2);
  tft.println("    On Smartphone first");
}
void ShowQR_APSetup(){
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(3, 3);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(true);
  tft.setTextSize(3);
  tft.print("\n\n");
  tft.setTextSize(2);
  //tft.println("      Please connect");
  tft.drawBitmap(60, 20, wifiQR, 200, 200, ST77XX_WHITE);
}
void ShowNightMode(){
  tft.fillScreen(ST77XX_BLACK);
  tft.drawBitmap(101, 61, night128px, 108, 108, ST77XX_WHITE);
  delay(1000);
  tft.fillScreen(ST77XX_BLACK);
}
void WifiConnected (String ssid){
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(3, 3);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(true);
  tft.setTextSize(3);
  tft.print("\n\n\n");
  tft.setTextSize(2);
  tft.println("       Connected to");
  tft.setTextSize(4);
  tft.print("\n  ");
  tft.println(ssid);
}
void MainUI_Border(uint16_t color){
  
  tft.drawRect(0,0,320,240,color);
  tft.drawRect(20,20,280,200,color);
  
  tft.fillRect(0,0,320,20,color);
  tft.fillRect(0,0,20,240,color);
  tft.fillRect(0,220,320,20,color);
  tft.fillRect(300,0,20,240,color);
  
  //tft.fillRect(21,21,280,200,ST77XX_BLACK);
}/*
void MainUI(int pm2_5, int temp, int humi){
  //tft.fillRect(21,21,279,199,ST77XX_BLACK);
  tft.fillRect(21,69,275,70,ST77XX_BLACK);
  tft.fillRect(88, 175,40,20,ST77XX_BLACK);
  tft.fillRect(180,175,40,20,ST77XX_BLACK);
  if(pm2_5 <= 50){
    MainUI_Border(ST77XX_GREEN);
    Blynk.virtualWrite(V9,"Good");
  }
  else if(pm2_5 > 50 && pm2_5 <= 100){
    MainUI_Border(ST77XX_YELLOW);
    Blynk.virtualWrite(V9,"Moderate");
  }
  else if(pm2_5 > 100 && pm2_5 <= 150){
    MainUI_Border(ST77XX_RED);
    Blynk.virtualWrite(V9,"Unhealthy");
  }
  else if(pm2_5 > 150){
    MainUI_Border(ST77XX_MAGENTA);
    Blynk.virtualWrite(V9,"Hazardous");
  }
  tft.setCursor(3, 3);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(true);
  tft.setTextSize(3);
  tft.print("\n");
  tft.setTextSize(2);
  tft.println("          PM 2.5 ");
  tft.setTextSize(2);
  tft.print("\n\n");
  if(isPMRead == false){
    tft.setTextSize(4);
    tft.print("    ");
    tft.setTextSize(1);
    tft.print(" ");
    tft.setTextSize(7);
    tft.println("---");
  }
  else{
    if(pm2_5 < 10){
    tft.setTextSize(4);
    tft.print("    ");
    tft.setTextSize(1);
    tft.print(" ");
    tft.setTextSize(7);
    tft.println("00" + String(pm2_5));
  }
  else if(pm2_5 >= 10 && pm2_5 < 100){
    tft.setTextSize(4);
    tft.print("    ");
    tft.setTextSize(1);
    tft.print(" ");
    tft.setTextSize(7);
    tft.println("0" + String(pm2_5));
  }
  else if(pm2_5 >= 100 && pm2_5 < 999){
    tft.setTextSize(4);
    tft.print("    ");
    tft.setTextSize(1);
    tft.print(" ");
    tft.setTextSize(7);
    tft.println(pm2_5);
  }
  else if (pm2_5 >= 1000){
    tft.setTextSize(4);
    tft.print("   ");
    tft.setTextSize(1);
    tft.print(" ");
    tft.setTextSize(7);
    tft.println(pm2_5);
  }
  }
  tft.setTextSize(2);
  tft.println("\n           ug/m3\n");
  tft.print("     T: ");
  if(isDHTRead == false){
    tft.print("--");
  }
  else{
    tft.print(temp);
  }
  tft.print(" C");
  tft.print(" H: ");
  if(isDHTRead == false){
    tft.print("--");
  }
  else{
    tft.print(humi);
  }
  tft.print(" %");
  showModeIcon(currMode);
}*/

void MainUI(int pm2_5, int temp, int humi){
  //tft.fillRect(21,21,279,199,ST77XX_BLACK);
  tft.fillRect(21,69,275,70,ST77XX_BLACK);
  tft.fillRect(70, 175,40,20,ST77XX_BLACK);
  tft.fillRect(165,175,40,20,ST77XX_BLACK);
  if(pm2_5 <= 50){
    MainUI_Border(ST77XX_GREEN);
    Blynk.virtualWrite(V9,"Good");
  }
  else if(pm2_5 > 50 && pm2_5 <= 100){
    MainUI_Border(ST77XX_YELLOW);
    Blynk.virtualWrite(V9,"Moderate");
  }
  else if(pm2_5 > 100 && pm2_5 <= 150){
    MainUI_Border(ST77XX_RED);
    Blynk.virtualWrite(V9,"Unhealthy");
  }
  else if(pm2_5 > 150){
    MainUI_Border(ST77XX_MAGENTA);
    Blynk.virtualWrite(V9,"Hazardous");
  }
  tft.setCursor(3, 3);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(true);
  tft.setTextSize(3);
  tft.print("\n");
  tft.setTextSize(2);
  tft.println("                 ");
  tft.setTextSize(2);
  tft.print("\n");
  if(isPMRead == false){
    tft.setTextSize(4);
    tft.print("    ");
    tft.setTextSize(1);
    tft.print(" ");
    tft.setTextSize(7);
    tft.println("---");
  }
  else{
    if(pm2_5 < 10){
    tft.setTextSize(4);
    tft.print("    ");
    tft.setTextSize(1);
    tft.print(" ");
    tft.setTextSize(7);
    tft.println("00" + String(pm2_5));
  }
  else if(pm2_5 >= 10 && pm2_5 < 100){
    tft.setTextSize(4);
    tft.print("    ");
    tft.setTextSize(1);
    tft.print(" ");
    tft.setTextSize(7);
    tft.println("0" + String(pm2_5));
  }
  else if(pm2_5 >= 100 && pm2_5 < 999){
    tft.setTextSize(4);
    tft.print("    ");
    tft.setTextSize(1);
    tft.print(" ");
    tft.setTextSize(7);
    tft.println(pm2_5);
  }
  else if (pm2_5 >= 1000){
    tft.setTextSize(4);
    tft.print("   ");
    tft.setTextSize(1);
    tft.print("");
    tft.setTextSize(7);
    tft.println(pm2_5);
  }
  }
  tft.setTextSize(1);
  tft.println("\n                \n");
  tft.print("   ");
  tft.setTextSize(1);
  tft.print(" ");
  tft.setTextSize(2);
  tft.print("   ");
  tft.setTextSize(2);
  if(isDHTRead == false){
    tft.print("--");
  }
  else{
    tft.print(temp);
  }
  tft.print(" C");
  tft.print("    ");
  if(isDHTRead == false){
    tft.print("--");
  }
  else{
    tft.print(humi);
  }
  tft.print(" %");
  showModeIcon(currMode);
  if(WiFi.status()== WL_CONNECTED){
    ShowWifiIcon();
  }
  else{
    tft.fillRect(255,175,22,22,ST77XX_BLACK);
    Blynk.connect();
  }
}

void PMMeasure(){
  String input = Serial.readStringUntil('\n');
  if(input.startsWith("PM")){
    input.remove(0,2); //Remove word "PM"
    if(input == "Cannot find the data header."){
      isPMRead = false;
    }
    else{
      int con_pm2_5 = input.toInt();
      if(con_pm2_5 == -1){
        isPMRead = false;
      }
      else{
        pm2_5 = con_pm2_5;
        isPMRead = true;
      Blynk.virtualWrite(V1,pm2_5);
      }
    }
    //Serial.println(pm2_5);
  }
    //isPMRead = false;
  delay(500);
}
void TempandHumu(){

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    isDHTRead = false;
    return;
  }
  if((t >= -55 && t <= 55) && (h >= 0 && h <= 100)){
    isDHTRead = true;
    Blynk.virtualWrite(V2,t);
    Blynk.virtualWrite(V3,h);
  }
  else{
    isDHTRead = false;
    Blynk.virtualWrite(V2,NULL);
    Blynk.virtualWrite(V3,NULL);
  }
  /*
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.println(F("°F "));*/
  delay(1000);
}
void drawBitmap(){
  tft.drawBitmap(220, 180, wifiOn, 24, 24, ST77XX_WHITE);
}
void ShowAutoMode(){
  tft.fillScreen(ST77XX_BLACK);
  tft.drawBitmap(91, 51, auto128px, 128, 128, ST77XX_WHITE);
  delay(1000);
  tft.fillScreen(ST77XX_BLACK);
  Blynk.virtualWrite(V6,1);
  Blynk.virtualWrite(V7,0);
  Blynk.virtualWrite(V8,0);
}
void ShowTurboMode(){
  tft.fillScreen(ST77XX_BLACK);
  tft.drawBitmap(91, 51, turbo128px, 128, 128, ST77XX_WHITE);
  delay(1000);
  tft.fillScreen(ST77XX_BLACK);
  Blynk.virtualWrite(V6,0);
  Blynk.virtualWrite(V7,1);
  Blynk.virtualWrite(V8,0);
}
void showModeIcon(int _mode){
  if(_mode == 1){
    tft.drawBitmap(226, 175, autoMode, 24, 24, ST77XX_WHITE);
  }
  else if(_mode == 2){
    tft.drawBitmap(227, 172, turboMode, 24, 24, ST77XX_WHITE);
  }
  else if(_mode == 3){
    tft.drawBitmap(228, 175, nightMode, 20, 20, ST77XX_WHITE);
  }
}
void ShowWifiIcon(){
  tft.drawBitmap(255, 175, wifiOn, 22, 22, ST77XX_WHITE);
}
void pmToFanMode(){
  int pmMode = pm2_5 / 20;
  Serial.print("FAN");
    switch(pmMode){
    case 0: Serial.println(100); delay(250); fanSpeed = 5; break;
    case 1: Serial.println(101); delay(250); fanSpeed = 10; break;
    case 2: Serial.println(102); delay(250); fanSpeed = 20; break;
    case 3: Serial.println(103); delay(250); fanSpeed = 30; break;
    case 4: Serial.println(104); delay(250); fanSpeed = 40; break;
    case 5: Serial.println(105); delay(250); fanSpeed = 50; break;
    case 6: Serial.println(106); delay(250); fanSpeed = 60; break;
    case 7: Serial.println(107); delay(250); fanSpeed = 70; break;
    case 8: Serial.println(108); delay(250); fanSpeed = 80; break;
    case 9: Serial.println(109); delay(250); fanSpeed = 90; break;
    case 10: Serial.println(110); delay(250); fanSpeed = 100; break;
  }
  if(pmMode > 10){
    Serial.println(110); fanSpeed = 100;
  }
  Blynk.virtualWrite(V5,fanSpeed);
  delay(250);
}
void manFanMode(int _mode){
  Serial.print("FAN");
  switch(_mode){
    case 0: Serial.println(100); delay(250); fanSpeed = 5; break;
    case 1: Serial.println(101); delay(250); fanSpeed = 10; break;
    case 2: Serial.println(102); delay(250); fanSpeed = 20; break;
    case 3: Serial.println(103); delay(250); fanSpeed = 30; break;
    case 4: Serial.println(104); delay(250); fanSpeed = 40; break;
    case 5: Serial.println(105); delay(250); fanSpeed = 50; break;
    case 6: Serial.println(106); delay(250); fanSpeed = 60; break;
    case 7: Serial.println(107); delay(250); fanSpeed = 70; break;
    case 8: Serial.println(108); delay(250); fanSpeed = 80; break;
    case 9: Serial.println(109); delay(250); fanSpeed = 90; break;
    case 10: Serial.println(110); delay(250); fanSpeed = 100; break;
  }
  Blynk.virtualWrite(V5,fanSpeed);   
  delay(250);
}
