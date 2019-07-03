//Pins
const int rs = 8;
const int en = 9;
const int d4 = 4;
const int d5 = 5;
const int d6 = 6;
const int d7 = 7;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


void display_setup()
{
	lcd.begin(16, 2);
}

void display_update()
{
	//DateTime now = ClockManager.getNow();
	char tim[16];
	sprintf(tim, "Time %02d:%02d:%02d", nowTime.hour(), nowTime.minute(), nowTime.second());
	char dat[16];
	sprintf(dat, "Date %04d/%02d/%02d", nowTime.year(), nowTime.month(), nowTime.day());
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print(tim);
	lcd.setCursor(0, 1);
	lcd.print(dat);
}