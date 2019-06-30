/*
  LiquidCrystal Library - Hello World

 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch prints "Hello World!" to the LCD
 and shows the time.

  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe
 modified 7 Nov 2016
 by Arturo Guadalupi

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystalHelloWorld

*/
#include <stdio.h>

// include the library code:
#include <LiquidCrystal.h>
#include <Wire.h>
#include "RTClib.h"

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

//const int rs = 11, en = 12, d4 = 7, d5 = 8, d6 = 9, d7 = 10;    //1st
const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;         //shield
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//sensor data
DateTime now;

//display things
int seconds = 0;
String lineBlank  = "                ";
int displayCounter = 0;
const unsigned long dt = 50;                    //time resolution
const unsigned long displayCycleTime = 3000;     //time per screen
const int dispLim = displayCycleTime / dt;

void setup() {
  //------------------------- DEBUG
  Serial.begin(9600);
  //while (!Serial) {
  //  ; // wait for serial port to connect. Needed for native USB port only
  //}
  //------------------------- DEBUG
  
  lcd.begin(16, 2);

  while (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
  
    // Comment out below lines once you set the date & time.
    // Following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
    // Following line sets the RTC with an explicit date & time
    // for example to set January 27 2017 at 12:56 you would call:
    // rtc.adjust(DateTime(2017, 1, 27, 12, 56, 0));
  }
}

void loop() {
  //time div
  unsigned long t0 = (int)(millis()/dt);

  //------------------------- DEBUG
  //serial timing log
  char buf[16];
  ltoa(millis(),buf,10);
  Serial.write(buf);
  Serial.write("\n");
  //------------------------- DEBUG

  //Poll Sensors:
  sensorPoll();
  
  //display
  lcd_check();

  //log
  logger();
  
  //wait until next time division
  while(((millis()/dt) == t0)){}
}

void lcd_check(){
  displayCounter++;
  if (displayCounter == dispLim){
    lcd_page1();
  }
  else if (displayCounter == 2*dispLim){
    lcd_displayTimeAndDate();
    displayCounter = 0;
  }
}

void lcd_page1(){
  seconds = millis()/1000;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Seconds:");
  lcd.setCursor(0, 1);
  lcd.print(seconds);
}

void lcd_displayTimeAndDate(){
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

void sensorPoll(){
  //clock
  clock_rtc();
  
  //float level sensor
  //radio RX-1
  //radio RX-2
  //reset button
  //manual override
  //manual power
}

void transmit(bool sig){
  
}

void logger(){
  
}

void clock_rtc(){
  now = rtc.now();  
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
