void control_update()
{
	if (input_manualOverride)
	{
		if (input_manualPower)
		{
			//pump should be on
			state_transferPumpIntention = true;
		}
		else 
		{
			//pump should be off
			state_transferPumpIntention = false;
		}
	}
	else 
	{
		if (input_ecoMode)
		{
			if (!state_isPeakTime)
			{
				if (input_waterLevel)
				{
					//pump should be off
					state_transferPumpIntention = false;
				}
				else
				{
					//pump should be on
					state_transferPumpIntention = true;
				}
			}
			else
			{
				//pump should be off
				state_transferPumpIntention = false;
			}
		}
		else
		{
			if (input_waterLevel)
			{
				//pump should be off
				state_transferPumpIntention = false;
			}
			else
			{
				//pump should be on
				state_transferPumpIntention = true;
			}
		}
	}
}

