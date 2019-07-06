//Input Pins
const int pin_manualOverride = 30;
const int pin_manualPower = 31;
const int pin_ecoMode = 32;
const int pin_floatSwitch = 33;
const int pin_rxA = 19;
const int pin_rxB = 17;

//Output pins
const int pin_txA = 18;
const int pin_txB = 16;
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
	pinMode(pin_rxA, INPUT);
	pinMode(pin_rxB, INPUT);
	
	pinMode(pin_txA, OUTPUT);
	pinMode(pin_txB, OUTPUT);
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
	input_floatSwitch 		= digitalRead(pin_floatSwitch);
	input_rxA 				= digitalRead(pin_rxA);
	input_rxB 				= digitalRead(pin_rxB);
}

void output_update()
{
	digitalWrite(pin_txA, output_txA);
	digitalWrite(pin_txB, output_txB);
	
	if (flag_pumpStatusKnown)
	{
		digitalWrite(pin_pumpStatusGreen, !flag_pumpPower);
		digitalWrite(pin_pumpStatusRed, flag_pumpPower);
	}
	else 
	{
		digitalWrite(pin_pumpStatusGreen, true);
		digitalWrite(pin_pumpStatusRed, true);
	}
	
	if (input_rxA || input_rxB)
	{		
		digitalWrite(pin_receiving, false);
		digitalWrite(pin_transmitting, true);
	}
	else 
	{
		digitalWrite(pin_receiving, true);
		if (output_txA || output_txB) 	digitalWrite(pin_transmitting, false);
		else							digitalWrite(pin_transmitting, true);
	}
	
	
	
	
	digitalWrite(pin_waterLevelGreen, !flag_waterLevel);
	digitalWrite(pin_waterLevelRed, flag_waterLevel);
	
	digitalWrite(pin_commsStatusGreen, flag_commError);
	digitalWrite(pin_commsStatusRed, !flag_commError);
	
	digitalWrite(pin_sdCardStatusGreen, flag_logError);
	digitalWrite(pin_sdCardStatusRed, !flag_logError);
}