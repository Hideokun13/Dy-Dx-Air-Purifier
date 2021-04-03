#define BLYNK_PRINT Serial
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(D3,D4); // RX, TX
LiquidCrystal_I2C lcd(0x27, 20, 4);
unsigned int pm1 = 0;
unsigned int pm2_5 = 0;
unsigned int pm10 = 0;
bool isInputArrFull = false;
int input[10];

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "CgLdjb2rxmaPX7gZsbnkHoEOiyoV0QVM"; //dydx
//char auth[] = "vB8IO3nZpycpEY3yrUfSIGZBxTQ78pKt";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = ".Pvpeez13";
char pass[] = "0912010056";
//char ssid[] = "aapl";
//char pass[] = "12345678";
void setup()
{
  // Debug console
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass, IPAddress(185,78,164,48), 8080); //dydx
  //Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);
  lcd.begin();
  WelcomeSplash();
  while (!Serial) ;
  mySerial.begin(9600);
}

void WelcomeSplash(){
  lcd.setCursor(1,0);
  lcd.print("PM 2.5 Tester v1.0");
  lcd.setCursor(6,1);
  lcd.print("by Dy/Dx");
  delay(2000);
  lcd.setCursor(5,2);
  lcd.print("Start in 3");
  delay(1000);
  lcd.setCursor(5,2);
  lcd.print("Start in 2");
  delay(1000);
  lcd.setCursor(5,2);
  lcd.print("Start in 1");
  delay(1000);
  lcd.clear();
}
void PrintPM(unsigned int pm_input){
  lcd.setCursor(2,0);
  lcd.print("PM 2.5 = ");
  lcd.print(pm_input);
  lcd.print(" ug/m3");
  lcd.setCursor(4,1);
  lcd.print("-----------");
  lcd.setCursor(1,2);
  lcd.print("0  |");
  lcd.setCursor(5,2);
  lcd.print("          ");
  lcd.setCursor(5,2);
  if(pm_input <= 10){
    lcd.print("");
  }
  else if(pm_input >= 10 && pm_input < 20){
    lcd.print(".");
  }
  else if(pm_input >= 20 &&pm_input < 30){
    lcd.print("..");
  }
  else if(pm_input >= 30 && pm_input < 40){
    lcd.print("...");
  }
  else if(pm_input >= 40 && pm_input < 50){
    lcd.print("....");
  }
  else if(pm_input >= 50 && pm_input < 60){
    lcd.print(".....");
  }
  else if(pm_input >= 60 &&pm_input < 70){
    lcd.print("......");
  }
  else if(pm_input >= 70 && pm_input < 80){
    lcd.print(".......");
  }
  else if(pm_input >= 80 && pm_input < 90){
    lcd.print("........");
  }
  else if(pm_input >= 90 && pm_input < 100){
    lcd.print(".........");
  }
  else if(pm_input >= 100){
    lcd.print("..........");
  }
  lcd.setCursor(14,2);
  lcd.print("| 100");
  lcd.setCursor(4,3);
  lcd.print("-----------");
  delay(500);
}


void loop()
{
  Blynk.run();
  
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
//Serial.print("{ ");
//Serial.print("\"pm1\": ");
//Serial.print(pm1);
//Serial.print(" ug/m3");
//Serial.print(", ");
}
else if (index == 7) {
pm2_5 = 256 * previousValue + value;
Blynk.virtualWrite(V1,pm2_5);
Serial.print("\"pm2_5\": ");
Serial.print(pm2_5);
Serial.print(" ug/m3");
Serial.print(", ");
PrintPM(pm2_5);
}
else if (index == 9) {
pm10 = 256 * previousValue + value;
//Serial.print("\"pm10\": ");
//Serial.print(pm10);
//Serial.print(" ug/m3");
} else if (index > 15) {
break;
}
index++;
}
while(mySerial.available()) mySerial.read();
Serial.println(" }");
delay(1000);
}
