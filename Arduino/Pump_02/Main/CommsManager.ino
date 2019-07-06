unsigned long transmitTime = 3000L;		//ms
unsigned long receiveTime = 1000L;		//ms

unsigned long commTxATimeStart = 0;
unsigned long commTxBTimeStart = 0;
unsigned long commRxATimeStart = 0;
unsigned long commRxBTimeStart = 0;
unsigned long t = 0;

void comms_update()
{
	if (input_rxA != comm_rxA) commRxATimeStart = millis();
	if (input_rxB != comm_rxB) commRxBTimeStart = millis();
	comm_rxA = input_rxA;
	comm_rxB = input_rxB;
	
	if (comm_txA)
	{
		t = millis();
		if (t - commTxATimeStart < transmitTime)
		{
			//transmitting
			comm_txA = true;
			comm_txB = false;
		}
		else
		{
			//done transmitting
			comm_txA = false;
			comm_txB = false;
			state_pumpStatusKnown = true;
		}
	}
	
	if (comm_txB)
	{
		t = millis();
		if (t - commTxBTimeStart < transmitTime)
		{
			//transmitting
			comm_txB = true;
			comm_txA = false;
		}
		else
		{
			//done transmitting
			comm_txB = false;
			comm_txA = false;
			state_pumpStatusKnown = true;
		}
	}
	
	if (comm_rxA)
	{
		//echo
		//comms_startPump();
		comm_txA = true;
		comm_txB = false;
		t = millis();
		//echo
		if (t - commRxATimeStart > receiveTime)
		{
			//received success
			state_pumpIntention = true;
		}
	}
	else commRxATimeStart = millis();
	
	if (comm_rxB)
	{
		//echo
		//comms_stopPump();
		comm_txB = true;
		comm_txA = false;
		t = millis();
		//echo
		if (t - commRxBTimeStart > receiveTime)
		{
			//recieved success
			state_pumpIntention = false;
		}
	}
	else commRxBTimeStart = millis();
	
	output_txA = comm_txA;
	output_txB = comm_txB;
	
}

void comms_updateState()
{
	if (!state_pumpStatusKnown)
	{
		if (state_pumpIntention)
		{
			comms_startPump();
		}
		else if (!state_pumpIntention)
		{
			comms_stopPump();
		}
	}
}

void comms_startPump()
{
	//cancel off-signal
	comm_txB = true;
	
	//send power-on signal
	if (!comm_txA)
	{
		Serial.println("tx pump on");
		comm_txA = true;
		commTxATimeStart = millis();
		commRxATimeStart = millis();
	}
}

void comms_stopPump()
{
	//cancel on-signal
	comm_txA = false;
	
	//send power-off signal
	if (!comm_txB)
	{
		Serial.println("tx pump off");
		comm_txB = true;
		commTxBTimeStart = millis();
		commRxBTimeStart = millis();
	}
}