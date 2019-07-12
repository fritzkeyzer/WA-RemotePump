void control_update()
{
	if (input_manualOverride)
	{
		if (input_manualPower)
		{
			//pump should be on
			state_pumpIntention = true;
		}
		else 
		{
			//pump should be off
			state_pumpIntention = false;
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
					state_pumpIntention = false;
				}
				else
				{
					//pump should be on
					state_pumpIntention = true;
				}
			}
			else
			{
				//pump should be off
				state_pumpIntention = false;
			}
		}
		else
		{
			if (input_waterLevel)
			{
				//pump should be off
				state_pumpIntention = false;
			}
			else
			{
				//pump should be on
				state_pumpIntention = true;
			}
		}
	}
}

