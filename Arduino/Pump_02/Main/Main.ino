/*
Fritz Keyzer July 2019
fritzkeyzer@gmail.com

For Carel Kostense
carel@waterworksautomation.co.za
www.waterworksautomation.co.za
*/

//#include <stdio.h>


//settings
unsigned long mediumInterval = 5000;		//state handling interval (ms)



//input states
bool input_rxA = false;
bool input_rxB = false;

//output states
bool output_txA = false;
bool output_txB = false;
bool output_pumpOn = false;
bool output_pumpOff = false;

//comm states
bool comm_txA = false;
bool comm_txB = false;
bool comm_rxA = false;
bool comm_rxB = false;

//control states:
bool state_pumpIntention = false;
bool state_pumpStatusKnown = false;

void setup() 
{
	Serial.begin(9600);
	Serial.println("Serial Connected. Hello");
	
	io_setup();
	
	loop_fast();
	loop_medium();
}

void loop() 
{
	unsigned long loop_medTimer = millis();
	while(millis() - loop_medTimer < mediumInterval)
	{
		loop_fast();
	}
	loop_medium();
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
	//comms state handler	- set flags to transmit - translate received flags - reset flags
	comms_updateState();
}