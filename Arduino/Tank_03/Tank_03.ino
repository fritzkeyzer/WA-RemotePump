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
const unsigned long displayCycleTime = 4000;     //time per screen  (ms)

//------------------------- SETTINGS

//sd card logging
File logFile;
const String fileName = "log.csv";
const int pinCS = 53;
bool loggingError = false;
bool loggingErrorPrev = true;

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
const int loggingErrorRedLED    = 22;
const int loggingErrorGreenLED  = 23;
const int floatSensorPin        = 31;
const int resetButtonPin        = 30;
const int potPin                = A8;
const int rxReceivedPin         = 26;
const int rxAPin                = 26;
const int rxBPin                = 26;
const int rxCPin                = 26;
const int rxDPin                = 26;
const int txAPin                = 26;
const int txBPin                = 26;
const int txCPin                = 26;
const int txDPin                = 26;


//sensor data
bool devicePower = false;
bool devicePowerPrev = false;
bool floatSensor = false;
bool floatSensorPrev = false;
bool resetButton = false;
bool resetButtonPrev = false;
int pot = 0;
int potPrev = 0;
bool manual_override = false;
bool manual_overridePrev = false;
bool manual_power = false;
bool manual_powerPrev = false;
bool pump_instruction = false;
bool pump_instructionPrev = false;
bool pump_running = false;
bool pump_runningPrev = false;

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

  //scan for events
  eventDetector();

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
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
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
    loggingError = false;
  }
  else {
    Serial.println("SD card connection failed");
    loggingError = true;
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
  if (loggingError) {
    Serial.println("Attempting to reconnect sd card");
    sd_setup();
  }

  if (!loggingError) {
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
      loggingError = true;
      Serial.println("Error writing to log");
    }
  }


}

void lcd_check() {
  displayCounter++;
  if (displayCounter == dispLim) {
    lcd_page_pump();
  }
  else if (displayCounter == 2*dispLim) {
    lcd_page_timeAndDate();
    if (!loggingError) displayCounter = 0;
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

void lcd_page_pump() {
  lcd.clear();
  lcd.setCursor(0, 0);
  if      (!manual_override) lcd.print("PUMP - auto");
  else                      lcd.print("PUMP - manual");
  lcd.setCursor(0, 1);
  char since_buf[16];
  if (pump_running) sprintf(since_buf, "on since %02d:%02d", pumpSince.hour(), pumpSince.minute());
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
  lcd.print("LOG ERROR since:");
  lcd.setCursor(0, 1);
  char buf[16];
  sprintf(buf, "%04d/%02d/%02d %02d:%02d", logErrorSince.year(), logErrorSince.month(), logErrorSince.day(), logErrorSince.hour(), logErrorSince.minute());
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
  pinMode(loggingErrorRedLED, OUTPUT);
  pinMode(loggingErrorGreenLED, OUTPUT);
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

void eventDetector() {
  //device power
  if (devicePower && !devicePowerPrev)              logEvent("power", "on");
  devicePowerPrev = devicePower;

  //float sensor
  if (floatSensor && !floatSensorPrev)              logEvent("water_level", "high");
  if (!floatSensor && floatSensorPrev)              logEvent("water_level", "low");
  floatSensorPrev = floatSensor;

  //logging error
  if (loggingError && !loggingErrorPrev){
    digitalWrite(loggingErrorRedLED, HIGH);
    digitalWrite(loggingErrorGreenLED, LOW);
    logErrorSince = now;
  }
  if (!loggingError && loggingErrorPrev){
    digitalWrite(loggingErrorRedLED, LOW);
    digitalWrite(loggingErrorGreenLED, HIGH);
  }
  loggingErrorPrev = loggingError;

  //radio RX

  //radio TX

  //reset button
  if (resetButton && !resetButtonPrev)                logEvent("reset", "pressed");
  resetButtonPrev = resetButton;

  //manual override
  if (manual_override && !manual_overridePrev)        logEvent("mode", "manual");
  if (!manual_override && manual_overridePrev)        logEvent("mode", "auto");
  manual_overridePrev = manual_override;
  
  //manual power
  if (manual_power && !manual_powerPrev)              logEvent("manual_override", "pump_on");
  if (!manual_power && manual_powerPrev)              logEvent("manual_override", "pump_off");
  manual_powerPrev = manual_power;

  //pump instruction status
  if (pump_instruction && !pump_instructionPrev)      logEvent("transmission", "pump_on");
  if (!pump_instruction && pump_instructionPrev)      logEvent("transmission", "pump_off");
  pump_instructionPrev = pump_instruction;
  
  //pump running status
  if (pump_running && !pump_runningPrev)             logEvent("pump_running", "on");  pumpSince = now;
  if (!pump_running && pump_runningPrev)             logEvent("pump_running", "off"); pumpSince = now;
  pump_runningPrev = pump_running;

  

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
