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

//pump controller settings
const unsigned long pumpSwitchTime = 5000;      //time before pump switches on or off (ms)
const unsigned long pumpErrorRetryTime = 20000;  //time after an error before retrying to start or stop pump (ms)

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
DateTime waterLevelSince;
DateTime commsErrorSince;

//lcd things
//const int rs = 11, en = 12, d4 = 7, d5 = 8, d6 = 9, d7 = 10;    // pins
const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;         // shield pins
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);                        //lcd initialise

//display variables
int displayCounter = 0;
const int dispLim = displayCycleTime / dt;

//comms variables
bool tx_a = false;
bool tx_aPrev = false;
bool tx_b = false;
bool tx_bPrev = false;
int tx_a_counter = 0;
int tx_b_counter = 0;
int rx_a_counter = 0;
int rx_b_counter = 0;
const int transmitLim = 3000 / dt;
const int receiveLim = 1000 / dt;

//pump control variables
int pumpSwitchCounter = 0;
const int pumpSwitchLim = pumpSwitchTime / dt;
int pumpErrorCounter = 0;
int pumpNoErrorCounter = 0;
const int pumpErrorLim = receiveLim;
int pumpErrorRetryCounter = 0;
const int pumpErrorRetryLim = pumpErrorRetryTime / dt;
bool intention = false;
bool intentionPrev = false; 

//PINS:
const int loggingErrorRedLED    = 22;
const int loggingErrorGreenLED  = 23;
const int floatSensor_pin       = 30;
const int resetButton_pin       = 31;
const int manual_override_pin   = 34;
const int manual_power_pin      = 35;
const int rx_received_pin       = 26;
const int rx_a_pin              = 19;
const int rx_b_pin              = 17;
const int tx_a_pin              = 18;
const int tx_b_pin              = 16;


//sensor data
bool devicePower = false;
bool devicePowerPrev = false;
bool floatSensor = false;
bool floatSensorPrev = false;
bool resetButton = false;
bool resetButtonPrev = false;
bool rx_received = false;
bool rx_a = false;
bool rx_b = false;
bool rx_c = false;
bool rx_d = false;
bool rx_aPrev = false;
bool rx_bPrev = false;

//states
bool manual_override = false;
bool manual_overridePrev = false;
bool manual_power = false;
bool manual_powerPrev = false;
bool pump_instruction = false;
bool pump_running = false;
bool comms_error = false;
bool comms_errorPrev = false;




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

  //pump logic
  pumpController();

  //comms
  transmit();
  receive();

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
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
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

      logErrorSince = now;
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
  if (displayCounter == dispLim){
    lcd_page_waterLevel();
  }
  else if (displayCounter == 2*dispLim) {
    lcd_page_pump();
  }
  else if (displayCounter == 3*dispLim) {
    lcd_page_timeAndDate();
    if (!loggingError && !comms_error) displayCounter = 0;
  }
  else if (displayCounter == 4*dispLim) {
    if (loggingError){
      lcd_page_loggingError();
      if (!comms_error) displayCounter = 0;
    }
    else {
      lcd_page_commsError();
      displayCounter = 0;
    }
  }
  else if (displayCounter == 5*dispLim){
    lcd_page_commsError();
    displayCounter = 0;
  }
}

void lcd_page_waterLevel() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WATER LEVEL:");   
  lcd.setCursor(0, 1);
  char since_buf[16];
  if (floatSensor)  sprintf(since_buf, "HI since %02d:%02d", pumpSince.hour(), pumpSince.minute());
  else              sprintf(since_buf, "LO since %02d:%02d", pumpSince.hour(), pumpSince.minute());
  lcd.print(since_buf);
  
}

void lcd_page_pump() {
  lcd.clear();
  lcd.setCursor(0, 0);
  if      (!manual_override)  lcd.print("PUMP - auto");
  else                        lcd.print("PUMP - manual");
  lcd.setCursor(0, 1);
  char since_buf[16];
  if (pump_running) sprintf(since_buf, "on since %02d:%02d", pumpSince.hour(), pumpSince.minute());
  else              sprintf(since_buf, "off since %02d:%02d", pumpSince.hour(), pumpSince.minute());
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

void lcd_page_commsError(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("COMMS ERROR since:");
  lcd.setCursor(0, 1);
  char buf[16];
  sprintf(buf, "%04d/%02d/%02d %02d:%02d", commsErrorSince.year(), commsErrorSince.month(), commsErrorSince.day(), commsErrorSince.hour(), commsErrorSince.minute());
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
  pinMode(floatSensor_pin, INPUT);
  pinMode(resetButton_pin, INPUT);
  pinMode(manual_override_pin, INPUT);
  pinMode(manual_power_pin, INPUT);
  pinMode(rx_received_pin, INPUT);
  pinMode(rx_a_pin, INPUT);
  pinMode(rx_b_pin, INPUT);
  pinMode(tx_a_pin, INPUT);
  pinMode(tx_b_pin, INPUT);
}

void sensorPoll() {
  //clock
  clock_rtc();

  //float level sensor
  floatSensor = digitalRead(floatSensor_pin);

  //reset button
  resetButton = digitalRead(resetButton_pin);
  
  //manual
  manual_override = digitalRead(manual_override_pin);
  manual_power = digitalRead(manual_power_pin);

  //radio RX
  rx_received = digitalRead(rx_received_pin);
  rx_a = digitalRead(rx_a_pin);
  rx_b = digitalRead(rx_b_pin);
}

void transmit() {
  if (pump_instruction){
    digitalWrite(tx_b_pin, LOW);
    tx_b = false;
    if (tx_a_counter < transmitLim){
      tx_a_counter++;
      digitalWrite(tx_a_pin, HIGH);
      tx_a = true;
    }
    else {
      digitalWrite(tx_a_pin, LOW);
      tx_a = false;
    }
  }
  else {
    digitalWrite(tx_a_pin, LOW);
    tx_a = false;
    if (tx_b_counter < transmitLim){
      tx_b_counter++;
      digitalWrite(tx_b_pin, HIGH);
      tx_b = true;
    }
    else {
      digitalWrite(tx_b_pin, LOW);
      tx_b = false;
    }
  }
}

void receive(){
  if (rx_a){
    rx_a_counter ++;  
    if (rx_a_counter > receiveLim){
      pump_running = true;  
    }
  }
  else {
    rx_a_counter = 0;
  }
  if (rx_b){
    rx_b_counter ++;  
    if (rx_b_counter > receiveLim){
      pump_running = false;  
    }
  }
  else {
    rx_b_counter = 0;
  }

  //Error detector
  if ((rx_a && tx_a) || (rx_b && tx_b)){
    if (pumpNoErrorCounter > pumpErrorLim){
      comms_error = false;
      pumpErrorCounter = 0;
    }
    else {
      pumpNoErrorCounter++;
    }
  }
  if ((!rx_a && tx_a) || (!rx_b && tx_b)){
    pumpNoErrorCounter = 0;
    //Serial.println(String(pumpErrorCounter));
    if (pumpErrorCounter > pumpErrorLim){
      comms_error = true;
      commsErrorSince = now;
    }
    else {
      pumpErrorCounter++;
    }
  }
}

void eventDetector() {
  //device power
  if (devicePower && !devicePowerPrev)                logEvent("power", "on");
  devicePowerPrev = devicePower;

  //float sensor
  if (floatSensor && !floatSensorPrev){               logEvent("water_level", "high");  
    waterLevelSince = now;
    }
  if (!floatSensor && floatSensorPrev){               logEvent("water_level", "low");   
    waterLevelSince = now;
  }
  floatSensorPrev = floatSensor;

  //logging error
  if (loggingError && !loggingErrorPrev){
    digitalWrite(loggingErrorRedLED, HIGH);
    digitalWrite(loggingErrorGreenLED, LOW);
  }
  if (!loggingError && loggingErrorPrev){
    digitalWrite(loggingErrorRedLED, LOW);
    digitalWrite(loggingErrorGreenLED, HIGH);
  }
  loggingErrorPrev = loggingError;

  //reset button
  if (resetButton && !resetButtonPrev)                logEvent("reset", "pressed");
  resetButtonPrev = resetButton;

  //manual override
  if (manual_override && !manual_overridePrev)        logEvent("mode", "manual");
  if (!manual_override && manual_overridePrev)        logEvent("mode", "auto");
  manual_overridePrev = manual_override;
  
  //manual power
  if (manual_power && !manual_powerPrev)              logEvent("manual_override", "pump_start");
  if (!manual_power && manual_powerPrev)              logEvent("manual_override", "pump_stop");
  manual_powerPrev = manual_power;

  //pump instruction status
  if (tx_a && !tx_aPrev){                             logEvent("tx", "pump_start"); 
    tx_a_counter = 0; 
  }
  tx_aPrev = tx_a;
  
  if (tx_b && !tx_bPrev){                             logEvent("tx", "pump_stop");
    tx_b_counter = 0;
  }
  tx_bPrev = tx_b;
  
  //pump running status
  if (rx_a && !rx_aPrev){                             logEvent("rx", "pump_running");  
    pumpSince = now;
  }
  rx_aPrev = rx_a;
  
  if (rx_b && !rx_bPrev){                             logEvent("rx", "pump_stopped");  
    pumpSince = now;
  }
  rx_bPrev = rx_b;

  //pump error
  if (comms_error && !comms_errorPrev)                  logEvent("rx", "error_echo_failed");
  if (!comms_error && comms_errorPrev)                  logEvent("rx", "error_resolved");
  comms_errorPrev = comms_error;
  
}

void logEvent(String event, String value) {
  lcd_page_event(event, value);
  sd_logEvent(event, value);
}

void pumpController(){
  //pump intention control
  if (manual_override){
    if (manual_power){
      intention = true;  
    }
    else {
      intention = false; 
    }
  }
  else {
    if (!floatSensor){
      intention = true;  
    }
    else {
      intention = false; 
    }
  }

  if (intention && !intentionPrev){
    pumpSwitchCounter = 0;
    tx_a_counter = 0;
  }
  else if (!intention && intentionPrev){
    pumpSwitchCounter = 0;
    tx_b_counter = 0;
  }
  intentionPrev = intention;
  
  //pump_instruction delay
  if (intention != pump_instruction){
    //Serial.println(String(pumpSwitchCounter));
    if (pumpSwitchCounter > pumpSwitchLim){
      pump_instruction = intention;
    }
    else {
      pumpSwitchCounter++;
    }
  }

  //Error retry
  if (comms_error){
    if (pumpErrorRetryCounter > pumpErrorRetryLim){
      tx_a_counter = 0;
      tx_b_counter = 0;
      pumpErrorRetryCounter = 0;
      //comms_error = false;
      pumpErrorCounter = 0;
      //logEvent("error", "retrying_transmission");
      Serial.println("retrying transmission");
    }
    else {
      pumpErrorRetryCounter++;
    }
  }
  else {
    pumpErrorRetryCounter = 0;
  }
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
