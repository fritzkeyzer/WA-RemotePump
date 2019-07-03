void control_update()
{
	if (state_waterLevel != input_floatSwitch) time_waterLevelSince = time_now;
	state_waterLevel = input_floatSwitch;
	
	state_manualOverride = input_manualOverride;
	state_manualPower = input_manualPower;
	state_ecoMode = input_ecoMode;
	
	
	if (state_manualOverride)
	{
		if (state_manualPower)
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
		if (state_ecoMode)
		{
			if (!state_isPeakTime)
			{
				if (state_waterLevel)
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
		else
		{
			if (state_waterLevel)
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

