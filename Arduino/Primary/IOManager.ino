//Input Pins
const int pin_manualOverride = 30;
const int pin_manualPower = 31;
const int pin_ecoMode = 32;
const int pin_floatSwitch = 33;

//Output pins
const int pin_pumpStatusGreen = 22;
const int pin_pumpStatusRed = 23;
const int pin_waterLevelGreen = 24;
const int pin_waterLevelRed = 25;
const int pin_commsStatusGreen = 26;
const int pin_commsStatusRed = 27;
const int pin_sdCardStatusGreen = 28;
const int pin_sdCardStatusRed = 29;
const int pin_transmitting = 35;
const int pin_receiving = 34;

void io_setup()
{
	pinMode(pin_manualOverride, INPUT);
	pinMode(pin_manualPower, INPUT);
	pinMode(pin_floatSwitch, INPUT);
	
	pinMode(pin_pumpStatusGreen, OUTPUT);
	pinMode(pin_pumpStatusRed, OUTPUT);
	pinMode(pin_waterLevelGreen, OUTPUT);
	pinMode(pin_waterLevelRed, OUTPUT);
	pinMode(pin_commsStatusGreen, OUTPUT);
	pinMode(pin_commsStatusRed, OUTPUT);
	pinMode(pin_sdCardStatusGreen, OUTPUT);
	pinMode(pin_sdCardStatusRed, OUTPUT);
	pinMode(pin_transmitting, OUTPUT);
	pinMode(pin_receiving, OUTPUT);
}

void input_update()
{
	input_manualOverride 	= digitalRead(pin_manualOverride);
	input_manualPower 		= digitalRead(pin_manualPower);
	input_ecoMode 			= digitalRead(pin_ecoMode);
	input_waterLevel 		= digitalRead(pin_floatSwitch);
}

void output_update()
{
	
	if (state_pumpStatusKnown)
	{
		digitalWrite(pin_pumpStatusGreen, !state_pumpPower);
		digitalWrite(pin_pumpStatusRed, state_pumpPower);
	}
	else 
	{
		digitalWrite(pin_pumpStatusGreen, true);
		digitalWrite(pin_pumpStatusRed, true);
	}
	
	if (output_rtxLed == 1)
	{
		//red
		digitalWrite(pin_receiving, true);
		digitalWrite(pin_transmitting, false);
	}
	else if (output_rtxLed == -1)
	{
		//green
		digitalWrite(pin_receiving, false);
		digitalWrite(pin_transmitting, true);
	}
	else 
	{
		digitalWrite(pin_receiving, true);
		digitalWrite(pin_transmitting, true);
	}
	
	
	
	
	digitalWrite(pin_waterLevelGreen, !input_waterLevel);
	digitalWrite(pin_waterLevelRed, input_waterLevel);
	
	digitalWrite(pin_commsStatusGreen, comm_error);
	digitalWrite(pin_commsStatusRed, !comm_error);
	
	digitalWrite(pin_sdCardStatusGreen, log_error);
	digitalWrite(pin_sdCardStatusRed, !log_error);
}