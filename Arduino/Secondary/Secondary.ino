/*
August 2019

Fritz Keyzer
fritzkeyzer@gmail.com

For Carel Kostense
carel@waterworksautomation.co.za
www.waterworksautomation.co.za
*/

/*

Secondary running on RF-NANO

Arduino IDE 1.8.5

*/

//custom libraries - also copied to libraries folder
#include "NRF_Fritz.h"
#include <SimpleThread.h> 


//settings
SimpleThread fastTimer(20);						//input and output states

//input states
bool input_riverPumpPower = false;
bool input_transferPumpPower = false;

//comms
bool comm_error = true;

//control states:
bool state_transferPumpIntention = false;
bool state_day = true;
CommsUnit::lowflowState state_lowflow = CommsUnit::OK;

void setup() 
{
	Serial.begin(9600);
	Serial.println("Serial Connected. Hello");
	Serial.println("Secondary Unit");
	
	io_setup();
	comms_setup();
	flow_setup();
}

void loop() 
{
	comms_update();
	
	if (fastTimer.check())
	{
		//input check			- toggle flags based off input readings
		input_update();
		
		//output set			- toggle control pins
		output_update();
		
		//alarm					- alarm routine
		alarm_update();			
	}
}