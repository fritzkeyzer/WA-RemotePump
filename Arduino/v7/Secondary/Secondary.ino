/*
Fritz Keyzer July 2019
fritzkeyzer@gmail.com

For Carel Kostense
carel@waterworksautomation.co.za
www.waterworksautomation.co.za
*/

#include "NRF_Fritz.h"
#include <SimpleThread.h> 


//settings
SimpleThread fastTimer(20);						//input and output states 
SimpleThread heartbeatTimer(5000);				//heartbeat pulse



//input states
bool input_pumpPower = false;

//output states
bool output_pumpOn = false;
bool output_pumpOff = false;

//comms
bool comm_error = false;


//control states:
bool state_pumpIntention = false;
bool state_pumpPower = false;

void setup() 
{
	Serial.begin(9600);
	Serial.println("Serial Connected. Hello");
	
	io_setup();
	comms_setup();
	//pumpController_setup();
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
		
		// pump controller		- send signals to pump
		//pumpController_update();
	}
	
	if (heartbeatTimer.check())
	{
		//Comms heartbeat		- send current intention as a reminder
		comms_heartbeat();
	}
}