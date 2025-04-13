/*
 * 
 * All the resources for this project: https://randomnerdtutorials.com/
 * Modified by Rui Santos
 * 
 * Created by FILIPEFLOP
 * 
 */
// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include "RTClib.h"
#include <SoftwareSerial.h>

#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SS_PIN 10
#define RST_PIN 9
#define ID_recognised 5
#define ID_Not_recognised 6
#define buzz 4

MFRC522 mfrc522(SS_PIN, RST_PIN);    // Create MFRC522 instance.
LiquidCrystal_I2C lcd(0x27, 20, 4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

RTC_DS1307 rtc;

char daysOfTheWeek[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
String day, month, year, second, minute, hour, dofweek, myDate, myTime;

//Create software serial object to communicate with SIM800L
SoftwareSerial mySerial(2, 3);  //SIM800L Tx & Rx is connected to Arduino #3 & #2
String sender_NO = "";          //Deewan MTN


void setup() {
  pinMode(buzz, OUTPUT);
  pinMode(ID_recognised, OUTPUT);
  pinMode(ID_Not_recognised, OUTPUT);
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
  if (!rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  //Comment or Uncomment the below line of code to reset the time to the latest time and date.
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  lcd.init();  // initialize the lcd
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(115200);
  //Begin serial communication with Arduino and SIM800L
  mySerial.begin(115200);
  SPI.begin();         // Initiate  SPI bus
  mfrc522.PCD_Init();  // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();
  lcd.setCursor(0, 0);
  lcd.print("       WELCOME      ");
  delay(500);
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("       WELCOME      ");
  delay(500);
  lcd.clear();
  lcd.setCursor(0, 2);
  lcd.print("       WELCOME      ");
  delay(500);
  lcd.clear();
  lcd.setCursor(0, 3);
  lcd.print("       WELCOME      ");
  delay(500);
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("INITIALIZING  ");
  delay(500);
  lcd.setCursor(0, 1);
  lcd.print("INITIALIZING.  ");
  delay(500);
  lcd.setCursor(0, 1);
  lcd.print("INITIALIZING..  ");
  delay(500);
  lcd.setCursor(0, 1);
  lcd.print("INITIALIZING...  ");
  delay(500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("         BY         ");
  lcd.setCursor(0, 1);
  lcd.print("NAME:POPOOLA WILLIAM");
  lcd.setCursor(0, 2);
  lcd.print("MAT NO: 19CJ025832  ");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Starting");
  delay(500);
  lcd.setCursor(0, 1);
  lcd.print("Starting.");
  delay(500);
  lcd.setCursor(0, 1);
  lcd.print("Starting..");
  delay(500);
  lcd.setCursor(0, 1);
  lcd.print("Starting...");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("RFID SECURITY SYSTEM");
  lcd.setCursor(0, 2);
  lcd.print("Scan card to proceed");
}
void loop() {

  DateTime now = rtc.now();
  year = String(now.year(), DEC);
  month = String(now.month(), DEC);
  day = String(now.day(), DEC);
  hour = String(now.hour(), DEC);
  minute = String(now.minute(), DEC);
  second = String(now.second(), DEC);
  dofweek = String(daysOfTheWeek[now.dayOfTheWeek()]);
  myDate = day + "/" + month + "/" + year;
  myTime = hour + ":" + minute + ":" + second;

  Serial.print(F("Date: "));
  Serial.print(myDate);
  Serial.print(F(" Time: "));
  Serial.print(myTime);
  Serial.print(F(" "));
  Serial.println(dofweek);
  Serial.println();
  delay(100);
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  // For the 1st parent
  if (content.substring(1) == "F3 5E 81 FB")  //change here the UID of the card/cards that you want to give access
  {
    digitalWrite(ID_recognised, HIGH);
    digitalWrite(buzz, HIGH);
    delay(250);
    digitalWrite(buzz, LOW);
    sender_NO = "+2348163180829";  //Deewan MTN
    Serial.println("Authorized access");
    Serial.println();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("RFID SECURITY SYSTEM");
    lcd.setCursor(0, 1);
    lcd.print("Card Detected!");
    delay(1000);
    lcd.setCursor(0, 2);
    lcd.print("Student Recognized!.");
    delay(2000);
    lcd.setCursor(0, 3);
    lcd.print("Student May Exit... ");
    delay(2000);
    msgBody();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("RFID SECURITY SYSTEM");
    lcd.setCursor(0, 2);
    lcd.print("Scan card to proceed");
    digitalWrite(ID_recognised, LOW);
  }
  // For the 2nd parent
  else if (content.substring(1) == "83 95 A6 0A")  //change here the UID of the card/cards that you want to give access
  {
    digitalWrite(ID_recognised, HIGH);
    digitalWrite(buzz, HIGH);
    delay(250);
    digitalWrite(buzz, LOW);
    sender_NO = "+2348163180829";  //Deewan MTN
    Serial.println("Authorized access");
    Serial.println();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("RFID SECURITY SYSTEM");
    lcd.setCursor(0, 1);
    lcd.print("Card Detected!");
    delay(1000);
    lcd.setCursor(0, 2);
    lcd.print("Student Recognized!.");
    delay(2000);
    lcd.setCursor(0, 3);
    lcd.print("Student May Exit... ");
    delay(2000);
    msgBody();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("RFID SECURITY SYSTEM");
    lcd.setCursor(0, 2);
    lcd.print("Scan card to proceed");
    digitalWrite(ID_recognised, LOW);
  }
  // For the 3rd parent
  else if (content.substring(1) == "03 5D B0 0A")  //change here the UID of the card/cards that you want to give access
  {
    digitalWrite(ID_recognised, HIGH);
    digitalWrite(buzz, HIGH);
    delay(250);
    digitalWrite(buzz, LOW);
    sender_NO = "+2348163180829";  //Deewan MTN
    Serial.println("Authorized access");
    Serial.println();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("RFID SECURITY SYSTEM");
    lcd.setCursor(0, 1);
    lcd.print("Card Detected!");
    delay(1000);
    lcd.setCursor(0, 2);
    lcd.print("Student Recognized!.");
    delay(2000);
    lcd.setCursor(0, 3);
    lcd.print("Student May Exit... ");
    delay(2000);
    msgBody();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("RFID SECURITY SYSTEM");
    lcd.setCursor(0, 2);
    lcd.print("Scan card to proceed");
    digitalWrite(ID_recognised, LOW);
  }

  else {
    digitalWrite(ID_recognised, LOW);
    digitalWrite(buzz, HIGH);
    digitalWrite(ID_Not_recognised, HIGH);
    Serial.println(" Access denied");
    sender_NO = "";  //Deewan MTN
    Serial.println("Authorized access");
    Serial.println();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("RFID SECURITY SYSTEM");
    lcd.setCursor(0, 1);
    lcd.print("Card Detected!");
    delay(1000);
    lcd.setCursor(0, 2);
    lcd.print("Student Identity is ");
    lcd.setCursor(0, 3);
    lcd.print("Not Recognised!!!.  ");
    delay(1300);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("RFID SECURITY SYSTEM");
    lcd.setCursor(0, 2);
    lcd.print("Scan card to proceed");
    digitalWrite(buzz, LOW);
    digitalWrite(ID_Not_recognised, LOW);
  }
}
void updateSerial() {
  delay(500);
  while (Serial.available()) {
    mySerial.write(Serial.read());  //Forward what Serial received to Software Serial Port
  }
  while (mySerial.available()) {
    Serial.write(mySerial.read());  //Forward what Software Serial received to Serial Port
  }
}

void msgBody() {
  mySerial.println("AT");  //Once the handshake test is successful, it will back to OK
  updateSerial();
  mySerial.println("AT+CMGF=1");  // Configuring TEXT mode
  updateSerial();
  mySerial.println("AT+CMGS=\"" + sender_NO + "\"");  //Mobile phone number to send message
  updateSerial();
  String SMS;
  SMS = "";
  //  SMS = "Hello,Time: " + hour + ":" + minute + "\rDate:" + day + "/" + month + "/" + year;
  SMS = "Attention!!!\nYour ward departed school premises on " + dofweek + ", " + myDate + " at time: " + myTime + ".";
  mySerial.print(SMS);  //text content
  // mySerial.print("Deewansonic Engineering");  //text content
  updateSerial();
  mySerial.write(26);
}
