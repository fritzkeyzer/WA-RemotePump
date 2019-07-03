#include <stdio.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include "RTClib.h"
#include <SPI.h>

RTC_DS3231 Clock;
DateTime initTime;
uint32_t initTimeSeconds;
DateTime nowTime;

void setup() 
{
	
	Wire.begin();
	
	Serial.begin(9600);
	Serial.println("hello");

	display_setup();
	
	initTime = Clock.now();
	initTimeSeconds = initTime.unixtime();
	
	Clock.adjust(DateTime(F(__DATE__), F(__TIME__)));
	
	clock_update();
	display_update();
}

void loop() 
{
	unsigned long milliSecs = millis();
	while(millis() - milliSecs < 3600000L){}
	clock_update();
	display_update();
}

void clock_update()
{
	unsigned long seconds = millis() / 1000;

	nowTime = Clock.now();
	
}