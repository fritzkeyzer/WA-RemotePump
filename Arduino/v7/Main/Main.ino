/*
Fritz Keyzer July 2019
fritzkeyzer@gmail.com

For Carel Kostense
carel@waterworksautomation.co.za
www.waterworksautomation.co.za
*/


#include <stdio.h>
#include <LiquidCrystal.h>
#include <Wire.h>
//#include "DS3231.h"
#include "RTClib.h"
#include <SD.h>
#include <SPI.h>
#include "NRF_Fritz.h"							//version=0.1.1
#include <SimpleThread.h> 

//settings
SimpleThread fastTimer(20);						//input and output states 
SimpleThread mediumTimer(1000);					//control logic


//times
DateTime time_now;
DateTime time_init;
//DateTime time_pumpSince;
//DateTime time_logErrorSince;
DateTime time_waterLevelSince;
//DateTime time_commsErrorSince;

//input states
bool input_manualOverride = false;
bool input_manualPower = false;
bool input_ecoMode = false;
bool input_waterLevel = false;
bool input_rxA = false;
bool input_rxB = false;

//output states
int output_rtxLed = 0;				// -1 for green, 0 for off, 1 for red

//comm states
bool comm_error = false;

//logger states
bool log_error = false;

//control states:
bool state_pumpIntention = false;
bool state_pumpPower = false;
bool state_isPeakTime = false;
bool state_pumpStatusKnown = false;

//event flags
bool flag_devicePower = false;
bool flag_manualOverride = false;
bool flag_manualPower = false;
bool flag_ecoMode = false;
bool flag_waterLevel = false;
bool flag_pumpIntention = false;
bool flag_pumpPower = false;
bool flag_commError = false;
bool flag_isPeakTime = false;
bool flag_logError = false;
bool flag_txOn = false;
bool flag_txOff = false;
bool flag_pumpStatusKnown = false;

void setup() 
{
	Serial.begin(9600);
	Serial.println("Serial Connected. Hello");
	
	comms_setup();
	clock_setup();
	display_setup();
	io_setup();
	event_setup();
}

void loop() 
{
	comms_update();
	
	if (fastTimer.check())
	{
		//input check			- read inputs
		input_update();
	
		//output set			- set output pins
		output_update();
		
		//display 				- check if display should scroll to next screen page
		display_update();
	}
	
	if (mediumTimer.check())
	{
		//time update			- update current time, update from rtc once per day
		clock_update();
		
		//control scheme		- determine whether pump should be pumping or not
		control_update();
		
		//event log				- log events to sd card, set flags equal to states
		event_update();
	}
}