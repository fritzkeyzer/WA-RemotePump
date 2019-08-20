//http://wiki.seeedstudio.com/2KM_Long_Range_RF_link_kits_w_encoder_and_decoder/

//settings:
SimpleThread heartbeatTimer(4000);				//heartbeat pulse
SimpleThread errorTime(60000);					//time after which no rx is considered an error
SimpleThread commLedTime(100);

const int radio_id = 1;
const int radio_cePin = 49;
const int radio_csnPin = 47;


CommsUnit radioUnit(radio_id, radio_cePin, radio_csnPin);

CommsUnit::message_s message;


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

void comms_heartbeat()
{
	output_rtxLed = 1;
	commLedTime.reset();
	
	/*
	if (state_transferPumpIntention)
	{
		radioUnit.sendMessage(1, CommsUnit::PUMP_TRANSFER_START);
		
	}
	else
	{
		radioUnit.sendMessage(1, CommsUnit::PUMP_TRANSFER_STOP);
	}
	if (state_isDay)
	{
		radioUnit.sendMessage(1, CommsUnit::TIME_DAY);
	}
	else
	{
		radioUnit.sendMessage(1, CommsUnit::TIME_NIGHT);
	}
	*/
	
	message.pumpIntention = state_transferPumpIntention;
	message.isDay = state_isDay;
	
	radioUnit.sendMessage(1, message);
}

/*
void comms_messageCallback(CommsUnit::message_e _msg)
{
	output_rtxLed = -1;
	commLedTime.reset();
	errorTime.reset();
	comm_error = false;
	state_pumpStatusKnown = true;
	Serial.println("RX: ");
	switch (_msg)
	{
		case CommsUnit::PUMP_RIVER_STOPPED:
			//Serial.println("River pump stopped");
			state_riverPumpPower = false;
		break;
		
		case CommsUnit::PUMP_RIVER_RUNNING:
			//Serial.println("River pump running");
			state_riverPumpPower = true;
		break;
		
		case CommsUnit::PUMP_TRANSFER_STOPPED:
			//Serial.println("Transfer pump stopped");
			state_transferPumpPower = false;
		break;
		
		case CommsUnit::PUMP_TRANSFER_RUNNING:
			//Serial.println("Transfer pump running");
			state_transferPumpPower = true;
		break;
		
		case CommsUnit::LOWFLOW_OK:
			//Serial.println("Lowflow OK");
			state_lowflow = OK;
		break;
		
		case CommsUnit::LOWFLOW_WARNING:
			//Serial.println("Lowflow warning");
			state_lowflow = WARNING;
		break;
		
		case CommsUnit::LOWFLOW_ALARM:
			//Serial.println("Lowflow alarm");
			state_lowflow = ALARM;
		break;
		
		default:
			Serial.println("unhandled message");
		break;
	}
	
}

*/

void comms_messageCallback(CommsUnit::message_s _msg)
{
	output_rtxLed = -1;
	commLedTime.reset();
	errorTime.reset();
	comm_error = false;
	state_pumpStatusKnown = true;
	//Serial.println("RX: ");
	
	state_riverPumpPower = _msg.riverPumpStatus;
	state_transferPumpPower = _msg.transferPumpStatus;
	state_lowflow = _msg.lowflowStatus;
}