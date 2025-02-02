//settings:
SimpleThread displayCycleTimer(4000);			//display cycle time

LiquidCrystal_I2C lcd(0x27,16,2);				//0x27 is default i2c address, 16,2 is lcd resolution: 16 characters, 2 rows

int currentScreen = 0;


void display_setup()
{
	//lcd.begin(16, 2);
	lcd.init();
	lcd.backlight();
}

void display_update()
{
	if (displayCycleTimer.check())
	{
		//display cycle			- scroll to next screen every few seconds
		display_cycle();
	}
}

void display_cycle()
{
	switch (currentScreen)
	{
		case -1:
			//event screen
			currentScreen++;
		break;
		
		case 0:
			display_screen_time();
			currentScreen++;
		break;
		
		case 1:
			display_screen_waterLevel();
			currentScreen++;
		break;
		
		case 2:
			display_screen_transferPumpStatus();
			currentScreen++;
		break;
		
		case 3:
			display_screen_ecoStatus();
			currentScreen++;
			if (comm_error)		currentScreen = 4;
			else if (log_error)	currentScreen = 5;
			else				currentScreen = 0;
			
		break;
		
		case 4:
			display_screen_commError();
			currentScreen++;
			if (log_error)		currentScreen = 5;
			else				currentScreen = 0;
		break;
		
		case 5:
			display_screen_logError();
			currentScreen++;
			currentScreen = 0;
		break;
	}
	
}

void display_screen_event(String event, String value)
{
	displayCycleTimer.reset();
	currentScreen = 0;
	
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print(event);
	lcd.setCursor(0, 1);
	lcd.print(value);
	
	delay(500);
}

void display_screen_time()
{
	lcd.clear();
	char tim[16];
	//sprintf(tim, "Time %02d:%02d:%02d", time_now.hour(), time_now.minute(), time_now.second());
	sprintf(tim, "Time %02d:%02d", time_now.hour(), time_now.minute());
	//sprintf(tim, "Time %02X:%02X:%02X", Clock.getHour(h12, PM), Clock.getMinute(), Clock.getSecond());
	char dat[16];
	sprintf(dat, "Date %04d/%02d/%02d", time_now.year(), time_now.month(), time_now.day());
	//sprintf(dat, "Date 20%02X/%02X/%02X", Clock.getYear(), Clock.getMonth(Century), Clock.getDate());
	lcd.setCursor(0, 0);
	lcd.print(tim);
	lcd.setCursor(0, 1);
	lcd.print(dat);
}

void display_screen_waterLevel()
{
	lcd.clear();
	lcd.setCursor(0, 0);
	if      (input_waterLevel)	lcd.print("Water: HIGH");
	else						lcd.print("Water: LOW");
	lcd.setCursor(0, 1);
	char since_buf[16];
	sprintf(since_buf, "Since: %02d:%02d", time_waterLevelSince.hour(), time_waterLevelSince.minute());
	lcd.print(since_buf);
}

void display_screen_transferPumpStatus()
{
	lcd.clear();
	lcd.setCursor(0, 0);
	if      (!input_manualOverride) lcd.print("Pump: AUTO");
	else                        	lcd.print("Pump: MANUAL");
	lcd.setCursor(0, 1);
	if (state_pumpStatusKnown)
	{
		if (state_transferPumpPower) 	lcd.print("RUNNING");
		else              				lcd.print("STOPPED");
	}
	else 
	{
		lcd.print("STATUS UNKNOWN");
	}
}

void display_screen_ecoStatus()
{
	lcd.clear();
	lcd.setCursor(0, 0);
	if      (input_ecoMode)	lcd.print("Eco: ENABLED");
	else					lcd.print("Eco: DISABLED");
	lcd.setCursor(0, 1);
	if      (state_isPeakTime)	lcd.print("Current: PEAK");
	else						lcd.print("Current: OFFPEAK");
}

void display_screen_commError()
{
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Comms error");
	//lcd.setCursor(0, 1);
	//TimeSpan dur = time_now - time_commsErrorSince;
	//char since_buf[16];
	//sprintf(since_buf, "For %03dd %02dh%02dm", dur.days(), dur.hours(), dur.minutes());
	//lcd.print(since_buf);
}

void display_screen_logError()
{
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Logging error");
	//lcd.setCursor(0, 1);
	//TimeSpan dur = time_now - time_logErrorSince;
	//char since_buf[16];
	//sprintf(since_buf, "For %03dd %02dh%02dm", dur.days(), dur.hours(), dur.minutes());
	//lcd.print(since_buf);
}