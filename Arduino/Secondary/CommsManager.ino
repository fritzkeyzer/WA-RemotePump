//http://wiki.seeedstudio.com/2KM_Long_Range_RF_link_kits_w_encoder_and_decoder/

SimpleThread errorTime(240000);
SimpleThread heartbeatTimer(1000);				//heartbeat pulse

const int radio_id = 1;
const int radio_cePin = 9;
const int radio_csnPin = 10;

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
		//Comms heartbeat		- send current status
		comms_sendUpdate();
	}
	
	if (errorTime.check())
	{
		comm_error = true;
		Serial.println("comm_error");
		state_transferPumpIntention = false;
	}
}

void comms_sendUpdate()
{
	message.riverPumpStatus = input_riverPumpPower;
	message.transferPumpStatus = input_transferPumpPower;
	message.lowflowStatus = state_lowflow;
	
	radioUnit.sendMessage(1, message);
}

void comms_messageCallback(CommsUnit::message_s _msg)
{
	errorTime.reset();
	comm_error = false;
	
	//Serial.println("RX: ");
	
	if (_msg.pumpIntention && !state_transferPumpIntention)
	{
		Serial.println("RX: pump should be ON");
	}
	else if (!_msg.pumpIntention && state_transferPumpIntention)
	{
		Serial.println("RX: pump should be OFF");
	}
	state_transferPumpIntention = _msg.pumpIntention;
	state_day = _msg.isDay;
}