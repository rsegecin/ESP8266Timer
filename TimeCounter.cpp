#include "TimeCounter.h"

TimeCounterClass::TimeCounterClass()
{
	pinMode(DEBUG_PIN, OUTPUT);

	bind_member<TimeCounterClass, &TimeCounterClass::timerCallback, 0> tCallBack(this);
	timer0_isr_init();
	timer0_attachInterrupt(tCallBack);
	timer0_write(ESP.getCycleCount() + 160000); // 80000 for 80Mhz
}

void TimeCounterClass::timerCallback()
{
	timer0_write(ESP.getCycleCount() + 160000); // 80000 for 80Mhz

	if (stateLED)
		stateLED = LOW;
	else
		stateLED = HIGH;

	digitalWrite(DEBUG_PIN, stateLED);

	TimeSpan.MiliSec++;
	TimeSpan.Ticks++;

	if (TimeSpan.MiliSec >= 1000)
	{
		TimeSpan.MiliSec = 0;
		TimeSpan.Seconds++;
		if (TimeSpan.Seconds >= 60)
		{
			TimeSpan.Seconds = 0;
			TimeSpan.Minutes++;
			if (TimeSpan.Minutes >= 60)
			{
				TimeSpan.Minutes = 0;
				TimeSpan.Hours++;
				if (TimeSpan.Hours >= 24)
				{
					TimeSpan.Hours = 0;
					TimeSpan.Days++;
				}
			}
		}
	}
}

void TimeCounterClass::DelayMili(uint16_t pMili)
{
	tmpTicks = TimeSpan.Ticks + pMili;

	while (TimeSpan.Ticks <= tmpTicks) { yield(); }
}

void TimeCounterClass::DelayMili(uint8_t pMili, bool &pFlag)
{
	tmpTicks = TimeSpan.Ticks + pMili;

	while ((!pFlag) && (TimeSpan.Ticks <= tmpTicks)) { yield(); }
}

void TimeCounterClass::DelayMili(uint8_t pMili, bool & pFlag, void(*doWhile)(void))
{
	tmpTicks = TimeSpan.Ticks + pMili;

	while ((!pFlag) && (TimeSpan.Ticks <= tmpTicks))
	{
		doWhile();
	}
}

void TimeCounterClass::ResetTimer()
{
	TimeSpan.MiliSec = 0;
	TimeSpan.Ticks = 0;
	TimeSpan.Seconds = 0;
	TimeSpan.Minutes = 0;
	TimeSpan.Hours = 0;
	TimeSpan.Days = 0;
}

TimeCounterClass TimeCounter;

