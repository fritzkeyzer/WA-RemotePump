#include <stdio.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include "RTClib.h"
#include <SD.h>
#include <SPI.h>

//------------------------- SETTINGS

//sensor settings
const unsigned long dt = 50;                    //time resolution (ms)

//display settings
const unsigned long displayCycleTime = 3000;     //time per screen  (ms)

//------------------------- SETTINGS

//sd card logging
File logFile;
const String fileName = "log.csv";
const int pinCS = 53;
bool sd_connected = false;
bool sd_connectedPrev = true;

//clock things
RTC_DS3231 rtc;   //clock object
const char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
DateTime now;     //time object
DateTime pumpSince;
DateTime logErrorSince;

//lcd things
//const int rs = 11, en = 12, d4 = 7, d5 = 8, d6 = 9, d7 = 10;    // pins
const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;         // shield pins
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);                        //lcd initialise

//PINS:
const int loggingErrorLED = 22;
const int floatSensorPin = 31;
const int resetButtonPin = 30;
const int potPin = A8;

//sensor data
bool devicePower = false;
bool devicePowerPrev = false;
bool floatSensor = false;
bool floatSensorPrev = false;
bool resetButton = false;
bool resetButtonPrev = false;
int pot = 0;
int potPrev = 0;
bool manualOverride = false;
bool pumpStatus = false;
bool pumpStatusPrev = false;

//lcd variables
int seconds = 0;
String lineBlank  = "                ";
int displayCounter = 0;
const int dispLim = displayCycleTime / dt;


void setup() {
  //------------------------- DEBUG
  Serial.begin(9600);
  //while (!Serial) {
  //  ; // wait for serial port to connect. Needed for native USB port only
  //}
  //------------------------- DEBUG

  //Clock
  clock_setup();

  //Pin setup
  pin_setup();

  //SD
  sd_setup();

  //LCD
  lcd.begin(16, 2);

  devicePower = true;
}

void loop() {
  //time div
  unsigned long t0 = millis();

  //------------------------- DEBUG
  //Serial.println((int)millis());
  //------------------------- DEBUG

  //Poll Sensors:
  sensorPoll();

  //display
  lcd_check();

  //log
  logger();

  //wait until next time division
  unsigned long t1 = millis();
  while ((t1 != t0) && !(t1 % dt == 0)) {
    t1 = millis();
  }
}

void clock_setup() {
  while (! rtc.begin()) {
    //Serial.println("Couldn't find RTC");
  }

  if (rtc.lostPower()) {
    //Serial.println("RTC lost power, lets set the time!");

    // Comment out below lines once you set the date & time.
    // Following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

    // Following line sets the RTC with an explicit date & time
    // for example to set January 27 2017 at 12:56 you would call:
    // rtc.adjust(DateTime(2017, 1, 27, 12, 56, 0));
  }
  clock_rtc();
  pumpSince = now;
}

void clock_rtc() {
  now = rtc.now();
}

void sd_setup() {
  pinMode(pinCS, OUTPUT);

  // SD Card Initialization
  if (SD.begin()) {
    Serial.println("SD card connected");
    sd_connected = true;
  }
  else {
    Serial.println("SD card connection failed");
    sd_connected = false;
    return;
  }

  if (!SD.exists(fileName)) {
    Serial.println("Log file does not exist");
    // Create/Open file
    logFile = SD.open(fileName, FILE_WRITE);
    if (logFile) {
      Serial.println("Creating log file");
      logFile.println("Date,Time,Event,Value");  // Write to file
      logFile.close();
    }
  }
  else {
    Serial.println("Existing log file found");
  }
}

void sd_logEvent(String event, String value) {
  if (!sd_connected) {
    Serial.println("Attempting to reconnect sd card");
    sd_setup();
  }

  if (sd_connected) {
    SD.exists(fileName);
    
    char dat[30];
    char tim[30];
    sprintf(dat, "%04d/%02d/%02d", now.year(), now.month(), now.day());
    sprintf(tim, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());

    // Create/Open file
    logFile = SD.open(fileName, FILE_WRITE);

    // if the file opened okay, write to it:
    if (logFile) {
      //Serial.println("Writing to file...");
      // Write to file
      logFile.print(dat);
      logFile.print(",");
      logFile.print(tim);
      logFile.print(",");
      logFile.print(event);
      logFile.print(",");
      logFile.println(value);
      logFile.close(); // close the file

      Serial.print("logging event: ");
      Serial.print(dat);
      Serial.print(" - ");
      Serial.print(tim);
      Serial.print(" - ");
      Serial.print(event);
      Serial.print(" - ");
      Serial.println(value);
    }
    // if the file didn't open, print an error:
    else {
      sd_connected = false;
      Serial.println("Error writing to log");
    }
  }


}

void lcd_check() {
  displayCounter++;
  if (displayCounter == dispLim) {
    lcd_page_pumpStatus();
  }
  else if (displayCounter == 2*dispLim) {
    lcd_page_timeAndDate();
    if (sd_connected) displayCounter = 0;
  }
  else if (displayCounter == 3*dispLim) {
    lcd_page_loggingError();
    displayCounter = 0;
  }
}

void lcd_page_waterLevel() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WATER LEVEL:");
  lcd.setCursor(0, 1);
   
  if (floatSensor){
     lcd.print("high");
  }
  else {
    lcd.print("low");
  }
  
}

void lcd_page_pumpStatus() {
  lcd.clear();
  lcd.setCursor(0, 0);
  if      (!manualOverride) lcd.print("PUMP - auto");
  else                      lcd.print("PUMP - manual");
  lcd.setCursor(0, 1);
  char since_buf[16];
  if (pumpStatus) sprintf(since_buf, "on since %02d:%02d", pumpSince.hour(), pumpSince.minute());
  else            sprintf(since_buf, "off since %02d:%02d", pumpSince.hour(), pumpSince.minute());
  lcd.print(since_buf);
}

void lcd_page_timeAndDate() {
  char tim[16];
  sprintf(tim, "Time %02d:%02d", now.hour(), now.minute());
  char dat[16];
  sprintf(dat, "Date %04d/%02d/%02d", now.year(), now.month(), now.day());
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(tim);
  lcd.setCursor(0, 1);
  lcd.print(dat);
}

void lcd_page_loggingError() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LOGGING ERROR!");
  lcd.setCursor(0, 1);
  long dif = now.secondstime() - logErrorSince.secondstime();
  long ss = dif % 60;
  dif /= 60; // now it is minutes
  long mm = dif % 60;
  dif /= 60; // now it is hours
  long hh = dif % 24;
  dif /= 24; // now it is days
  long dd = dif;
  
  char buf[16];
  sprintf(buf, "for %d days %02d:%02d", (int)dd, (int)hh, (int)mm);
  lcd.print(buf);
}

void lcd_page_event(String event, String value) {
  displayCounter = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(event);
  lcd.setCursor(0, 1);
  lcd.print(value);
}

void pin_setup() {
  pinMode(loggingErrorLED, OUTPUT);
  pinMode(floatSensorPin, INPUT);
  pinMode(resetButtonPin, INPUT);
  pinMode(potPin, INPUT);
}

void sensorPoll() {
  //clock
  clock_rtc();

  //float level sensor
  floatSensor = digitalRead(floatSensorPin);

  //radio RX-1
  //radio RX-2
  //reset button
  resetButton = digitalRead(resetButtonPin);
  
  //manual override
  //manual power

  //pot
  pot += 0.2f*((float)analogRead(potPin) - (float)pot);
}

void transmit(bool sig) {

}

void logger() {
  //device power
  if (devicePower && !devicePowerPrev)        logEvent("power", "on");
  devicePowerPrev = devicePower;

  //float sensor
  if (floatSensor && !floatSensorPrev)        logEvent("water_level", "high");
  if (!floatSensor && floatSensorPrev)        logEvent("water_level", "low");
  floatSensorPrev = floatSensor;

  //logging error
  if (sd_connected && !sd_connectedPrev){
    digitalWrite(loggingErrorLED, LOW);
  }
  if (!sd_connected && sd_connectedPrev){
    digitalWrite(loggingErrorLED, HIGH);
    logErrorSince = now;
  }
  sd_connectedPrev = sd_connected;

  //radio RX

  //radio TX

  //reset button
  if (resetButton && !resetButtonPrev)        logEvent("reset", "pressed");
  resetButtonPrev = resetButton;

  //manual override

  //manual power

  //pump status
  if (pumpStatus && !pumpStatusPrev){
    logEvent("pump", "on");
    pumpSince = now;
  }
  if (!pumpStatus && pumpStatusPrev){
    logEvent("pump", "off");
    pumpSince = now;
  }
  pumpStatusPrev = pumpStatus;

  //pot
  if (abs(pot - potPrev) > 10){
    logEvent("pot", String(pot));
    potPrev = pot;
  }
  
}

void logEvent(String event, String value) {
  lcd_page_event(event, value);
  sd_logEvent(event, value);
}


/*
  Serial.println("Current Date & Time: ");
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

  Serial.println("Unix Time: ");
  Serial.print("elapsed ");
  Serial.print(now.unixtime());
  Serial.print(" seconds/");
  Serial.print(now.unixtime() / 86400L);
  Serial.println(" days since 1/1/1970");


*/
