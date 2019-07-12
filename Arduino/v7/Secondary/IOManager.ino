//Input Pins
const int pin_pumpPower = A2;

//Output pins
const int pin_led = A3;
const int pin_pumpOn = A0;
//const int pin_pumpOff = A1;

void io_setup()
{
	pinMode(pin_pumpPower, INPUT);
	
	pinMode(pin_led, OUTPUT);
	pinMode(pin_pumpOn, OUTPUT);
	//pinMode(pin_pumpOff, OUTPUT);
}

void input_update()
{
	input_pumpPower = digitalRead(pin_pumpPower);
}

void output_update()
{
	//digitalWrite(pin_pumpOn, output_pumpOn);
	//digitalWrite(pin_pumpOff, output_pumpOff);
	
	if (!comm_error)
	{
		digitalWrite(pin_pumpOn, state_pumpIntention);
	}
	else 
	{
		digitalWrite(pin_pumpOn, false);
	}
	
	
	digitalWrite(pin_led, input_pumpPower);
}