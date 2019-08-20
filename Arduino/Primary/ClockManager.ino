//settings:
SimpleThread clockResetTimer(86400000);			//clock reset interval (24hrs in ms)


//DS3231 Clock;
RTC_DS3231 Clock;
uint32_t initTimeSeconds;

bool Century = false;
bool h12;
bool PM;
bool reset = false;

//peak 6am - 10am & 5pm - 8pm
const int pA_min = 6;			//morning peak period start time
const int pA_max = 10;			//morning peak period stop time
const int pB_min = 17;			//evening peak period start time
const int pB_max = 20;			//evening peak period stop time

const int day_min = 8;			//day time - for low flow alarm
const int day_max = 20;			//night time

void clock_setup()
{
	Century = false;
	reset = false;

	Wire.begin();
	
	//Serial.println("Wire started");
	
	if (! Clock.begin()) 
	{
		Serial.println("Couldn't find RTC Module");
		while (1);
	}
	
	if (Clock.lostPower()) 
	{
		Serial.println("RTC lost power, lets set the time!");
		Clock.adjust(DateTime(F(__DATE__), F(__TIME__)));
	}
	
	//if (Clock.lostPower())
	//{
	//	Serial.println("RTC lost power, lets set the time!");
	//	// following line sets the RTC to the date & time this sketch was compiled
	//	Clock.adjust(DateTime(F(__DATE__), F(__TIME__)));
	//	// This line sets the RTC with an explicit date & time, for example to set
	//	// January 21, 2014 at 3am you would call:
	//	// rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
	//}
	
	clock_resetTime();
	clock_update();
	
	//time_commsErrorSince = time_now;
	//time_pumpSince = time_now;
	time_waterLevelSince = time_now;
	//time_logErrorSince = time_now;
}

void clock_update()
{
	
	if (clockResetTimer.check())
	{
		//reset time			- reset the clock every 24hrs
		clock_resetTime();
	}
	
	
	unsigned long seconds = millis() / 1000;

	//nowTime = DateTime((uint32_t)initTimeSeconds + (uint32_t)seconds);
	
	//nowTime = RTClib::now();
	//nowTime = Clock.now();
	
	time_now = DateTime(seconds + initTimeSeconds);
	
	if (time_now.unixtime() < initTimeSeconds)
	{
		// second counter overflowed : reset
		clock_resetTime();
		Serial.println("Setting time");
	}
	
	clock_checkEcoTime();
	clock_checkDaytime();
}

static void clock_resetTime()
{
	//initTime = RTClib::now();
	//Clock.adjust(DateTime(F(__DATE__), F(__TIME__)));			//uncomment to set time... Keep commented thereafter...
	
	time_init = Clock.now();
	initTimeSeconds = time_init.unixtime();
	//initTimeSeconds = 0;
	reset = true;
}

void clock_checkEcoTime()
{
	int h = time_now.hour();
	if ((pA_min <= h && h < pA_max) || (pB_min <= h && h < pB_max))
	{
		state_isPeakTime = true;
	}
	else
	{
		state_isPeakTime = false;
	}
}

void clock_checkDaytime()
{
	bool ret = false;
	int h = time_now.hour();
	if (day_min <= h && h < day_max)
	{
		ret = true;
	}
	state_isDay = ret;
}