//Input Pins
const int pin_manualOverride = 34;
const int pin_manualPower = 35;
const int pin_ecoMode = 26;
const int pin_floatSwitch = 30;
const int pin_rxA = 19;
const int pin_rxB = 17;

//Output pins
const int pin_txA = 18;
const int pin_txB = 16;

void io_setup()
{
	pinMode(pin_manualOverride, INPUT);
	pinMode(pin_manualPower, INPUT);
	pinMode(pin_floatSwitch, INPUT);
	pinMode(pin_rxA, INPUT);
	pinMode(pin_rxB, INPUT);
	
	pinMode(pin_txA, OUTPUT);
	pinMode(pin_txB, OUTPUT);
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
}