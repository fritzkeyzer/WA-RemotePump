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

// include the library code:
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 11, en = 12, d4 = 7, d5 = 8, d6 = 9, d7 = 10;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int seconds = 0;
String lineBlank  = "                ";
int displayCounter = 0;
const unsigned long dt = 50;                    //time resolution
const unsigned long displayCycleTime = 1000;     //time per screen
const int dispLim = displayCycleTime / dt;

void setup() {
  //------------------------- DEBUG
  Serial.begin(9600);
  //while (!Serial) {
  //  ; // wait for serial port to connect. Needed for native USB port only
  //}
  //------------------------- DEBUG
  
  lcd.begin(16, 2);
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
    lcd_page2();
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

void lcd_page2(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Milliseconds:");
  lcd.setCursor(0, 1);
  lcd.print(millis());
}

void sensorPoll(){
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





