#include <stdio.h>
#include <LiquidCrystal.h>
#include <Wire.h>
//#include "DS3231.h"
#include "RTClib.h"
#include <SD.h>
#include <SPI.h>

//settings
unsigned long mediumInterval = 1000;		//state handling interval (ms)
unsigned long slowInterval = 4000;			//display page scroll interval (ms)


//times
DateTime time_now;
DateTime time_init;
DateTime time_pumpSince;
DateTime time_logErrorSince;
DateTime time_waterLevelSince;
DateTime time_commsErrorSince;

//input states
bool input_manualOverride = false;
bool input_manualPower = false;
bool input_ecoMode = false;
bool input_floatSwitch = false;
bool input_rxA = false;
bool input_rxB = false;

//output states
bool output_txA = false;
bool output_txB = false;

//comm states
bool comm_txA = false;
bool comm_txB = false;
bool comm_rxA = false;
bool comm_rxB = false;
bool comm_error = false;

//logger states
bool log_error = false;

//control states:
bool state_manualOverride = false;
bool state_manualPower = false;
bool state_ecoMode = false;
bool state_waterLevel = false;
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

//loop variables
unsigned long loop_slowTimer = millis();
unsigned long loop_medTimer = millis();

void setup() 
{
	Serial.begin(9600);
	Serial.println("Serial Connected. Hello");
	
	clock_setup();
	display_setup();
	io_setup();
	event_setup();
	
	loop_fast();
	loop_medium();
}

void loop() 
{
	loop_slowTimer = millis();
	while(millis() - loop_slowTimer < slowInterval)
	{
		loop_medTimer = millis();
		while(millis() - loop_medTimer < mediumInterval)
		{
			loop_fast();
		}
		loop_medium();
	}
	loop_slow();
}


void loop_fast()
{
	//input check			- toggle flags based off input readings
	input_update();
	
	//output set			- toggle control pins
	output_update();
	
	//comms manager			- reset flags - set flags for received data
	comms_update();
}

void loop_medium()
{
	//time update			- update current time, update from rtc once per hour
	clock_update();
	
	//control scheme		- determine whether pump should be pumping or not
	control_update();
	
	//event log				- log events to sd card, reset flags
	event_update();
	
	//comms state handler	- set flags to transmit - translate received flags - reset flags
	comms_updateState();
}

void loop_slow()
{
	//display update		- change display screen
	display_update();
}