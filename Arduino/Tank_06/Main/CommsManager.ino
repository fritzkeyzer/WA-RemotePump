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
			
			//if state doesnt match what we tried to send, there was an error
			if (!state_pumpPower)
			{
				//Serial.println("error");
				if (!comm_error) time_commsErrorSince = time_now;
				comm_error = true;
			}
			else state_pumpStatusKnown = true;
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
			//if state doesnt match what we tried to send, there was an error
			if (state_pumpPower)
			{
				//Serial.println("error");
				if (!comm_error) time_commsErrorSince = time_now;
				comm_error = true;
			}
			else state_pumpStatusKnown = true;
		}
	}
	
	if (comm_rxA)
	{
		t = millis();
		//echo
		if (t - commRxATimeStart > receiveTime)
		{
			//echo success
			if (!state_pumpPower) time_pumpSince = time_now;
			state_pumpPower = true;
			comm_error = false;
		}
	}
	else commRxATimeStart = millis();
	
	if (comm_rxB)
	{
		t = millis();
		//echo
		if (t - commRxBTimeStart > receiveTime)
		{
			//echo success
			if (state_pumpPower) time_pumpSince = time_now;
			state_pumpPower = false;
			comm_error = false;
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
	if (!comm_error)
	{
		if (state_pumpIntention && !state_pumpPower)
		{
			comms_startPump();
		}
		else if (!state_pumpIntention && state_pumpPower)
		{
			comms_stopPump();
		} 
	}
	else
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
	
	/*
	if (state_pumpIntention)
	{
		comms_startPump();
	}
	else
	{
		comms_stopPump();
	}
	*/
}

void comms_startPump()
{
	//cancel off-signal
	comm_txB = true;
	
	//send power-on signal
	if (!comm_txA)
	{
		//Serial.println("tx pump on");
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
		//Serial.println("tx pump off");
		comm_txB = true;
		commTxBTimeStart = millis();
		commRxBTimeStart = millis();
	}
}