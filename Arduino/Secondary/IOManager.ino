//Input Pins
const int pin_in_riverPumpPower = A0;
const int pin_in_transferPumpPower = A1;
const int pin_in_flowSensor = 2;

//Output pins
const int pin_out_led = LED_BUILTIN;
const int pin_out_siren = 3;
const int pin_out_pumpStart = 5;
const int pin_out_pumpStop = 4;
// 5

//flow rate things
float flow_currentFlowRate = 0;
SimpleThread timer_flowDebounce(50);
bool input_flowSensor = false;
bool input_flowSensorPrev = false;
unsigned long lastTick = millis();


//alarm things
const float flow_volumePerTick = 0.5;								// (in litres) litres per tick of flow sensor
const float flow_lowFlowThreshold = 150.0;						// (in litres per hour) Low flow rate threshold
SimpleThread timer_lowFlowAlarm(120000);						// (in ms) How long must the flow rate be below threshold before alarm is sounded?
SimpleThread timer_lowFlowDelay(60000);							// (in ms) How long should we wait after river pump comes on to check lowflow?
bool state_lowFlowDelay = true;

//siren things
SimpleThread timer_sirenPulseOn(500);							// (in ms) siren pulse length
const int siren_numberOfPulses = 3;								// number of pulses per alarm
SimpleThread timer_sirenAlarmInterval(3600000);					// (in ms) how long between alarms (roughly an hour?)
int counter_sirenPulses = 0;
bool state_siren = false;

//pump signal things
bool state_pumpStart = false;
bool state_pumpStop = false;
SimpleThread timer_pumpSignal(1000);
SimpleThread timer_pumpRetryTimer(5000);

void io_setup()
{
	pinMode(pin_in_riverPumpPower, INPUT_PULLUP);
	pinMode(pin_in_transferPumpPower, INPUT_PULLUP);
	pinMode(pin_in_flowSensor, INPUT_PULLUP);
	
	pinMode(pin_out_led, OUTPUT);
	pinMode(pin_out_siren, OUTPUT);
	pinMode(pin_out_pumpStart, OUTPUT);
	pinMode(pin_out_pumpStop, OUTPUT);
}

void input_update()
{
	if (!digitalRead(pin_in_riverPumpPower) != input_riverPumpPower)
	{
		Serial.print("river pump = ");
		Serial.println(!digitalRead(pin_in_riverPumpPower));
	}
	input_riverPumpPower = !digitalRead(pin_in_riverPumpPower);
	
	if (!digitalRead(pin_in_transferPumpPower) != input_transferPumpPower)
	{
		Serial.print("transfer pump = ");
		Serial.println(!digitalRead(pin_in_transferPumpPower));
	}
	input_transferPumpPower = !digitalRead(pin_in_transferPumpPower);
}

void output_update()
{
	digitalWrite(pin_out_siren, state_siren);
	digitalWrite(pin_out_led, state_transferPumpIntention);
	digitalWrite(pin_out_pumpStart, state_pumpStart);
	digitalWrite(pin_out_pumpStop, state_pumpStop);
	
	
	if (timer_pumpRetryTimer.check())
	{
		if (state_transferPumpIntention && !input_transferPumpPower)
		{
			pump_start();
		}
		else if (!state_transferPumpIntention && input_transferPumpPower)
		{
			pump_stop();
		}
	}
	if (timer_pumpSignal.check())
	{
		state_pumpStart = false;
		state_pumpStop = false;
	}
}

void pump_start()
{
	Serial.println("pump_start");
	timer_pumpSignal.reset();
	state_pumpStart = true;
	state_pumpStop = false;
}

void pump_stop()
{
	Serial.println("pump_stop");
	timer_pumpSignal.reset();
	state_pumpStart = false;
	state_pumpStop = true;
}

void flow_setup()
{
	attachInterrupt(digitalPinToInterrupt(pin_in_flowSensor), flow_interruptCallback, CHANGE);
	//timer_flowRateInterval.reset();
	timer_lowFlowDelay.reset();
	flow_interruptCallback();
}

void flow_interruptCallback()
{
	input_flowSensor = digitalRead(pin_in_flowSensor);
	if (timer_flowDebounce.check())
	{
		if (!input_flowSensor && input_flowSensorPrev)
		{
			flow_currentFlowRate = 3600000/(millis() - lastTick);
			Serial.print("Current flow rate (litres per hour): ");
			Serial.println(flow_currentFlowRate);
			lastTick = millis();
		}
	}
	input_flowSensorPrev = input_flowSensor;
}

void alarm_start()
{
	Serial.println("alarm_start");
	state_lowflow = CommsUnit::ALARM;
	counter_sirenPulses = 0;
	timer_sirenAlarmInterval.reset();
	timer_sirenPulseOn.reset();
}

void alarm_stop()
{
	if (state_lowflow == CommsUnit::ALARM)
	{
		Serial.println("alarm_stop");
	}
	state_lowflow = CommsUnit::OK;
	state_siren = false;
}

void alarm_update()
{
	if (input_riverPumpPower && state_day)
	{
		float rate = 3600000 / (millis() - lastTick);
		if (rate < flow_lowFlowThreshold)
		{
			flow_currentFlowRate = rate;
			//Serial.print("Rate (litres per hour): ");
			//Serial.print(rate);
			//Serial.print(" | delay = ");
			//Serial.println(state_lowFlowDelay);
		}
		
		if (timer_lowFlowDelay.check())
		{
			if (state_lowFlowDelay)
			{
				Serial.println("lowflow delay complete. Now checking lowflow");
			}
			state_lowFlowDelay = false;
		}
		
		//river pump is on. check that flow is ok.
		if (flow_currentFlowRate > flow_lowFlowThreshold)
		{
			// flow rate is good
			if (state_lowflow != CommsUnit::OK)
			{
				Serial.print("lowflow OK");
				Serial.print(" - Current flow rate (litres per hour): ");
				Serial.println(flow_currentFlowRate);
			}
			state_lowflow = CommsUnit::OK;
			alarm_stop();
		}
		else if (state_lowflow == CommsUnit::OK && state_lowFlowDelay == false)
		{
			// flow rate is bad && this is the first time && not still in delay
			if (state_lowflow != CommsUnit::WARNING)
			{
				Serial.print("lowflow WARNING");
				Serial.print(" - Current flow rate (litres per hour): ");
				Serial.println(flow_currentFlowRate);
				timer_lowFlowAlarm.reset();
			}
			
			state_lowflow = CommsUnit::WARNING;
		}
		
		if (state_lowflow == CommsUnit::WARNING)
		{
			// flow rate is bad && alarm hasnt yet been triggered
			if (timer_lowFlowAlarm.check())
			{
				//timer has elapsed, trigger alarm
				alarm_start();
			}
		}
	}
	else
	{
		alarm_stop();
		if (state_lowflow != CommsUnit::OK)
		{
			Serial.println("lowflow OK");
		}
		state_lowflow = CommsUnit::OK;
		timer_lowFlowDelay.reset();
		state_lowFlowDelay = true;
	}
	
	if (state_lowflow == CommsUnit::ALARM && !comm_error)
	{
		if (timer_sirenPulseOn.check())
		{
			//siren off
			//incremenent counter
			if (!state_siren && (counter_sirenPulses < siren_numberOfPulses))
			{
				state_siren = true;
				counter_sirenPulses++;
			}
			else if (state_siren)
			{
				state_siren = false;
			}
			
		}
		
		if (timer_sirenAlarmInterval.check())
		{
			Serial.println("alarm_repeat");
			counter_sirenPulses = 0;
		}
	}
	else
	{
		state_siren = false;
	}
}