//sd card logging
File logFile;
const String fileName = "log.csv";
const int pinCS = 53;

//https://howtomechatronics.com/tutorials/arduino/arduino-sd-card-data-logging-excel-tutorial/

void event_setup()
{
	pinMode(pinCS, OUTPUT);

	// SD Card Initialization
	if (SD.begin())
	{
		Serial.println("SD card connected");
		log_error = false;
	}
	else
	{
		Serial.println("SD card connection failed");
		log_error = true;
		return;
	}

	if (!SD.exists(fileName))
	{
		Serial.println("Log file does not exist");
		// Create/Open file
		logFile = SD.open(fileName, FILE_WRITE);
		if (logFile)
		{
			Serial.println("Creating log file");
			logFile.println("Date,Time,Event,Value");  // Write to file
			logFile.close();
		}
	}
	else
	{
		Serial.println("Existing log file found");
	}
}

void event_update()
{
	if (!flag_devicePower)
	{
		flag_manualOverride = !input_manualOverride;
		if (input_manualOverride) flag_manualPower 	= !input_manualPower;
		flag_ecoMode 		= !input_ecoMode;
		flag_waterLevel 	= !input_waterLevel;
	}
	
	//detect differences between flags and states
	if (!flag_devicePower)								event_log("POWER", "ON", true);
	
	if (input_manualOverride && !flag_manualOverride)	event_log("MODE", "MANUAL", true);
	if (!input_manualOverride && flag_manualOverride)	event_log("MODE", "AUTO", true);
	
	if (input_ecoMode && !flag_ecoMode)					event_log("ECO_MODE", "ON", true);
	if (!input_ecoMode && flag_ecoMode)					event_log("ECO_MODE", "OFF", true);
	
	if (state_isPeakTime && !flag_isPeakTime)			event_log("ECO_TIME", "PEAK", true);
	if (!state_isPeakTime && flag_isPeakTime)			event_log("ECO_TIME", "OFFPEAK", true);
	
	if (input_waterLevel && !flag_waterLevel)
	{													event_log("WATER_LEVEL", "HIGH", true);
		time_waterLevelSince = time_now;
	}
	if (!input_waterLevel && flag_waterLevel)
	{													event_log("WATER_LEVEL", "LOW", true);
		time_waterLevelSince = time_now;
	}
	
	if (input_manualPower && !flag_manualPower)			event_log("MANUAL", "PUMP_START", true);
	if (!input_manualPower && flag_manualPower)			event_log("MANUAL", "PUMP_STOP", true);
	
	if (state_pumpIntention && !flag_pumpIntention)		event_log("CONTROL", "PUMP_START", false);
	if (!state_pumpIntention && flag_pumpIntention)		event_log("CONTROL", "PUMP_STOP", false);
	
	if (comm_error && !flag_commError)					event_log("COMMS", "ERROR", true);
	if (!comm_error && flag_commError)					event_log("COMMS", "ERROR_RESOLVED", true);
	
	if (!log_error && flag_logError)					event_log("LOGGER", "ERROR_RESOLVED", false);
	
	if (!comm_error && flag_commError)
	{
		flag_pumpPower = !state_pumpPower;
	}
	if (state_pumpPower && !flag_pumpPower)
	{													event_log("PUMP", "RUNNING", true);
		//time_pumpSince = time_now;
	}
	if (!state_pumpPower && flag_pumpPower)
	{													event_log("PUMP", "STOPPED", true);
		//time_pumpSince = time_now;
	}
	
	//update flags
	flag_devicePower 	= true;
	flag_manualOverride = input_manualOverride;
	flag_manualPower 	= input_manualPower;
	flag_ecoMode 		= input_ecoMode;
	flag_waterLevel 	= input_waterLevel;
	flag_pumpPower 		= state_pumpPower;
	flag_commError		= comm_error;
	flag_pumpIntention	= state_pumpIntention;
	flag_isPeakTime		= state_isPeakTime;
}

void event_log(String event, String value, bool toDisplay)
{
	if (toDisplay) display_screen_event(event, value);
	event_logSD(event, value);
}

void event_logSD(String event, String value)
{
	if (log_error)
	{
		Serial.println("Attempting to reconnect sd card");
		event_setup();
	}

	if (!log_error)
	{
		SD.exists(fileName);

		char dat[30];
		char tim[30];
		sprintf(dat, "%04d/%02d/%02d", time_now.year(), time_now.month(), time_now.day());
		sprintf(tim, "%02d:%02d:%02d", time_now.hour(), time_now.minute(), time_now.second());

		// Create/Open file
		logFile = SD.open(fileName, FILE_WRITE);

		// if the file opened okay, write to it:
		if (logFile)
		{
			//Serial.println("Writing to file...");
			// Write to file
			logFile.print(dat);
			logFile.print(",");
			logFile.print(tim);
			logFile.print(",");
			logFile.print(event);
			logFile.print(",");
			logFile.println(value);
			logFile.close(); // close the file

			Serial.print("logging event: ");
			Serial.print(dat);
			Serial.print(" - ");
			Serial.print(tim);
			Serial.print(" - ");
			Serial.print(event);
			Serial.print(" - ");
			Serial.println(value);

			//time_logErrorSince = time_now;
		}
		// if the file didn't open, print an error:
		else
		{
			log_error = true;
			Serial.println("Error writing to log");
		}
	}
}