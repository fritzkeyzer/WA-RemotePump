//http://wiki.seeedstudio.com/2KM_Long_Range_RF_link_kits_w_encoder_and_decoder/

SimpleThread errorTime(125000);

const int radio_id = 1;
const int radio_cePin = 9;
const int radio_csnPin = 8;

CommsUnit radioUnit(radio_id, radio_cePin, radio_csnPin);


void comms_setup()
{
	radioUnit.init();
	radioUnit.attachMessageCallback(comms_messageCallback);
}

void comms_update()
{
	radioUnit.update();
	
	if (errorTime.check())
	{
		comm_error = true;
	}
}

void comms_pumpRunning()
{
	radioUnit.sendMessage(1, CommsUnit::PUMP_RUNNING);
}

void comms_pumpStopped()
{
	radioUnit.sendMessage(1, CommsUnit::PUMP_STOPPED);
}

void comms_heartbeat()
{
	if (input_pumpPower)
	{
		comms_pumpRunning();
	}
	else
	{
		comms_pumpStopped();
	}
}

void comms_messageCallback(CommsUnit::message_e _msg)
{
	errorTime.reset();
	comm_error = false;
	
	Serial.print("RX: ");
	switch (_msg)
	{
		case CommsUnit::PUMP_START:
			Serial.println("Start command");
			if (!state_pumpIntention)
			{
				//pumpController_pumpStart();
			}
			state_pumpIntention = true;
			comms_heartbeat();
		break;
		
		case CommsUnit::PUMP_STOP:
			Serial.println("Stop command");
			if (state_pumpIntention)
			{
				//pumpController_pumpStop();
			}
			state_pumpIntention = false;
			comms_heartbeat();
		break;
	}
	
}