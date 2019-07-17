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

void clock_setup()
{
	Century = false;
	reset = false;

	Wire.begin();
	
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
	}
	
	clock_checkEcoTime();
}

static void clock_resetTime()
{
	//initTime = RTClib::now();
	//Clock.adjust(DateTime(F(__DATE__), F(__TIME__)));			//keep this line commented out
	
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