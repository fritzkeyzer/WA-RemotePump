/*
  August 2019
  Last update - March 2020
  
  Fritz Keyzer
  fritzkeyzer@gmail.com
  
  For Carel Kostense
  carel@waterworksautomation.co.za
  www.waterworksautomation.co.za
*/

/*

  Primary running MCU-PRO Mega 2560
  
  Arduino IDE 1.8.10

*/

//builtin libraries
#include <stdio.h>
//#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <SPI.h>

//custom libraries - also copied to libraries folder
#include "RTClib.h"
#include <SD.h>
#include "NRF_Fritz.h"							//custom lib using NRFLite library
#include <SimpleThread.h> 

//settings
SimpleThread fastTimer(100);						//input and output states 
SimpleThread mediumTimer(5000);						//control logic

//times
DateTime time_now;
//DateTime time_init;
DateTime time_waterLevelSince;

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
bool state_transferPumpIntention = false;
bool state_riverPumpPower = false;
bool state_transferPumpPower = false;
bool state_isPeakTime = false;
bool state_pumpStatusKnown = false;

bool state_startup = true;
//enum lowflowState
//{
//	OK,
//	WARNING,
//	ALARM,
//};
CommsUnit::lowflowState state_lowflow = CommsUnit::OK;
CommsUnit::lowflowState flag_lowflow = CommsUnit::OK;
bool state_isDay = false;

//event flags
bool flag_devicePower = false;
bool flag_manualOverride = false;
bool flag_manualPower = false;
bool flag_ecoMode = false;
bool flag_waterLevel = false;
bool flag_transferPumpIntention = false;
bool flag_riverPumpPower = false;
bool flag_transferPumpPower = false;
bool flag_commError = false;
bool flag_isPeakTime = false;
bool flag_logError = false;
bool flag_txOn = false;
bool flag_txOff = false;
bool flag_pumpStatusKnown = false;
bool flag_isDay = false;

//flow logging
unsigned long flowCounter_thisHour = 0;					//clicks this hour
const float flow_volumePerTick = 100;					//amount of water represented by one tick

void setup() 
{
	Serial.begin(9600);
	Serial.println("Serial Connected. Hello");
	Serial.println("Primary Unit");
	
	comms_setup();
	//Serial.println("Comms Setup complete");
	clock_setup();
	//Serial.println("Clock Setup complete");
	display_setup();
	//Serial.println("LCD Setup complete");
	io_setup();
	//Serial.println("IO Setup complete");
	event_setup();
	//Serial.println("Event Setup complete");
	
	Serial.println("Setup complete");
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
