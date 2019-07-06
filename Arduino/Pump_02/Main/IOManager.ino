//Input Pins
const int pin_rxA = 2;
const int pin_rxB = 3;

//Output pins
const int pin_led = 13;
const int pin_txA = 4;
const int pin_txB = 5;
const int pin_pumpOn = 6;
const int pin_pumpOff = 7;



void io_setup()
{
	pinMode(pin_rxA, INPUT);
	pinMode(pin_rxB, INPUT);
	
	pinMode(pin_led, OUTPUT);
	pinMode(pin_txA, OUTPUT);
	pinMode(pin_txB, OUTPUT);
	pinMode(pin_pumpOn, OUTPUT);
	pinMode(pin_pumpOff, OUTPUT);
}

void input_update()
{
	input_rxA = digitalRead(pin_rxA);
	input_rxB = digitalRead(pin_rxB);
}

void output_update()
{
	digitalWrite(pin_txA, output_txA);
	digitalWrite(pin_txB, output_txB);
	
	digitalWrite(pin_pumpOn, output_txA);
	digitalWrite(pin_pumpOff, output_txB);
	
	digitalWrite(pin_led, state_pumpIntention);
}