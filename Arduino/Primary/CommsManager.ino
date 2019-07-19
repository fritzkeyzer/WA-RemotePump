//http://wiki.seeedstudio.com/2KM_Long_Range_RF_link_kits_w_encoder_and_decoder/

//settings:
SimpleThread heartbeatTimer(5000);				//heartbeat pulse
SimpleThread errorTime(60000);					//time after which no rx is considered an error
SimpleThread commLedTime(100);

const int radio_id = 1;
const int radio_cePin = 17;
const int radio_csnPin = 19;


CommsUnit radioUnit(radio_id, radio_cePin, radio_csnPin);


void comms_setup()
{
	radioUnit.init();
	radioUnit.attachMessageCallback(comms_messageCallback);
}

void comms_update()
{
	radioUnit.update();
	
	if (heartbeatTimer.check())
	{
		//Comms heartbeat		- send current intention as a reminder
		comms_heartbeat();
	}
	
	if (errorTime.check())
	{
		comm_error = true;
		state_pumpStatusKnown = false;
	}
	
	if (commLedTime.check())
	{
		output_rtxLed = 0;
	}
}

void comms_startPump()
{
	output_rtxLed = 1;
	commLedTime.reset();
	radioUnit.sendMessage(1, CommsUnit::PUMP_START);
	//errorTime.reset();
}

void comms_stopPump()
{
	output_rtxLed = 1;
	commLedTime.reset();
	radioUnit.sendMessage(1, CommsUnit::PUMP_STOP);
	//errorTime.reset();
}

void comms_heartbeat()
{
	if (state_pumpIntention)
	{
		comms_startPump();
	}
	else
	{
		comms_stopPump();
	}
}

void comms_messageCallback(CommsUnit::message_e _msg)
{
	output_rtxLed = -1;
	commLedTime.reset();
	errorTime.reset();
	comm_error = false;
	state_pumpStatusKnown = true;
	Serial.print("RX: ");
	switch (_msg)
	{
		case CommsUnit::PUMP_STOPPED:
			Serial.println("Other pump stopped");
			state_pumpPower = false;
		break;
		
		case CommsUnit::PUMP_RUNNING:
			Serial.println("Other pump running");
			state_pumpPower = true;
		break;
	}
	
}