//Front panel inputs
const int pin_manualOverride 			= A14;
const int pin_manualPower 				= 32;
const int pin_ecoMode 					= 34;

//IO inputs
const int pin_floatSwitch 				= 18;		
const int pin_flowMeter 				= 19;		//interrupt pin

//Output pins
const int pin_riverPumpStatusRed 		= A2;
const int pin_riverPumpStatusGreen 		= A3;
const int pin_transferPumpStatusRed 	= A4;
const int pin_transferPumpStatusGreen 	= A5;
const int pin_waterLevelRed 			= A6;
const int pin_waterLevelGreen 			= A7;
const int pin_commsStatusRed 			= A8;
const int pin_commsStatusGreen 			= A9;
const int pin_sdCardStatusRed 			= A10;
const int pin_sdCardStatusGreen 		= A11;
const int pin_transmitting 				= A12;
const int pin_receiving 				= A13;

//flow measurement
SimpleThread timer_flowDebounce(100);				//flow pin debounce
bool input_flowSensor = false;
bool input_flowSensorPrev = false;
unsigned long lastTick = millis();


void io_setup()
{
	pinMode(pin_manualOverride, INPUT_PULLUP);
	pinMode(pin_manualPower, INPUT_PULLUP);
	pinMode(pin_ecoMode, INPUT_PULLUP);
	pinMode(pin_floatSwitch, INPUT);
	pinMode(pin_flowMeter, INPUT);
	
	pinMode(pin_riverPumpStatusRed, OUTPUT);
	pinMode(pin_riverPumpStatusGreen, OUTPUT);
	pinMode(pin_transferPumpStatusRed, OUTPUT);
	pinMode(pin_transferPumpStatusGreen, OUTPUT);
	pinMode(pin_waterLevelRed, OUTPUT);
	pinMode(pin_waterLevelGreen, OUTPUT);
	pinMode(pin_commsStatusRed, OUTPUT);
	pinMode(pin_commsStatusGreen, OUTPUT);
	pinMode(pin_sdCardStatusRed, OUTPUT);
	pinMode(pin_sdCardStatusGreen, OUTPUT);
	pinMode(pin_transmitting, OUTPUT);
	pinMode(pin_receiving, OUTPUT);
	
	attachInterrupt(digitalPinToInterrupt(pin_flowMeter), flow_interruptCallback, CHANGE);
	flow_interruptCallback();
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
		digitalWrite(pin_riverPumpStatusGreen, state_riverPumpPower);
		digitalWrite(pin_riverPumpStatusRed, !state_riverPumpPower);
		
		digitalWrite(pin_transferPumpStatusGreen, state_transferPumpPower);
		digitalWrite(pin_transferPumpStatusRed, !state_transferPumpPower);
	}
	else 
	{
		digitalWrite(pin_riverPumpStatusGreen, false);
		digitalWrite(pin_riverPumpStatusRed, false);
		
		digitalWrite(pin_transferPumpStatusGreen, false);
		digitalWrite(pin_transferPumpStatusRed, false);
	}
	
	if (output_rtxLed == 1)
	{
		//red
		digitalWrite(pin_receiving, false);
		digitalWrite(pin_transmitting, true);
	}
	else if (output_rtxLed == -1)
	{
		//green
		digitalWrite(pin_receiving, true);
		digitalWrite(pin_transmitting, false);
	}
	else 
	{
		digitalWrite(pin_receiving, false);
		digitalWrite(pin_transmitting, false);
	}
	
	digitalWrite(pin_waterLevelGreen, input_waterLevel);
	digitalWrite(pin_waterLevelRed, !input_waterLevel);
	
	if (!state_startup)
	{
		digitalWrite(pin_commsStatusGreen, !comm_error);
		digitalWrite(pin_commsStatusRed, comm_error);
	}
	
	digitalWrite(pin_sdCardStatusGreen, !log_error);
	digitalWrite(pin_sdCardStatusRed, log_error);
}

void flow_interruptCallback()
{
	input_flowSensor = digitalRead(pin_flowMeter);
	if (timer_flowDebounce.check())
	{
		if (input_flowSensor && !input_flowSensorPrev)
		{
			flowCounter_thisHour ++;
			
			float rate = 3600000/(millis() - lastTick);
			Serial.print("Current flow rate (litres per hour): ");
			Serial.println(rate);
			lastTick = millis();
		}
	}
	input_flowSensorPrev = input_flowSensor;
}