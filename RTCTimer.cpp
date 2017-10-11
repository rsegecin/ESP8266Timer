#include "RTCTimer.h"

RTCTimerClass::RTCTimerClass()
{
	pinMode(DEBUG_PIN, OUTPUT);

	bind_member<RTCTimerClass, &RTCTimerClass::OnInterrupt, 0> tCallBack(this);
	timer0_isr_init();
	timer0_attachInterrupt(tCallBack);
	timer0_write(ESP.getCycleCount() + 160000); // 80000 for 80Mhz
}

void RTCTimerClass::OnInterrupt()
{
	timer0_write(ESP.getCycleCount() + 80000); // 80000 and 160000 for 80Mhz

	if (tmpTest)
	{
		tmpTest = false;
	}
	else
	{
		tmpTest = true;
	}

	digitalWrite(DEBUG_PIN, tmpTest);

	miliToSec++;
	miliSeconds++;

	if (miliToSec >= 1000)
	{
		miliToSec = 0;
		Time++;
	}
}

void RTCTimerClass::DelayMili(uint32_t pMili)
{
	tmpTicks = miliSeconds + pMili;

	while (miliSeconds <= tmpTicks) { yield(); }
}

void RTCTimerClass::DelayMili(uint32_t pMili, bool &pFlag)
{
	tmpTicks = miliSeconds + pMili;

	while ((!pFlag) && (miliSeconds <= tmpTicks)) { yield(); }
}

void RTCTimerClass::DelayMili(uint32_t pMili, void(*doWhile)(void))
{
	tmpTicks = miliSeconds + pMili;

	while (miliSeconds <= tmpTicks)
	{
		doWhile();
	}
}

void RTCTimerClass::DelayMili(uint32_t pMili, bool &pFlag, void(*doWhile)(void))
{
	tmpTicks = miliSeconds + pMili;

	while ((!pFlag) && (miliSeconds <= tmpTicks))
	{
		doWhile();
	}
}

void RTCTimerClass::SetTime(tTime pTime)
{
	Time = pTime;
}

void RTCTimerClass::SetTime(char pTimeString[])
{
	ParseTime(DateTime, pTimeString);
	Time = MakeTime(DateTime);
}

void RTCTimerClass::ParseTime(sDateTime &pDateTime, char pTimeString[])
{
	char aux[5];
	String strDateTime(pTimeString);

	int iT = strDateTime.indexOf('T');
	int iZ = strDateTime.indexOf('Z');

	if ((iT == 10) && ((iZ == 19) || (iZ == 23)))
	{
		strDateTime.substring(0, 4).toCharArray(aux, 5);
		pDateTime.Year = atoi(aux);
		strDateTime.substring(5, 7).toCharArray(aux, 3);
		pDateTime.Month = atoi(aux);
		strDateTime.substring(8, 10).toCharArray(aux, 3);
		pDateTime.DayOfMonth = atoi(aux);
		strDateTime.substring(11, 13).toCharArray(aux, 3);
		pDateTime.Hours = atoi(aux);
		strDateTime.substring(14, 16).toCharArray(aux, 3);
		pDateTime.Minutes = atoi(aux);
		strDateTime.substring(17, 19).toCharArray(aux, 3);
		pDateTime.Seconds = atoi(aux);
	}
}

tTime RTCTimerClass::MakeTime(sDateTime &pTm)
{
	int i;
	uint32_t seconds;
	int16_t year = pTm.Year - 2000;

	// seconds from 2000 till 1 jan 00:00:00 of the given year
	seconds = year * SECS_PER_YEAR;
	for (i = 0; i < year; i++)
	{
		if (LEAP_YEAR(i))
		{
			seconds += SECS_PER_DAY;
		}
	}

	// add days for this actual year passed on datetime, months start from 1
	for (i = 1; i < pTm.Month; i++)
	{
		switch (monthDays[i - 1])
		{
		case 28:
			if (LEAP_YEAR(year))
				seconds += SECS_FEB_LEAP;
			else
				seconds += SECS_FEB;
			break;
		case 30:
			seconds += SECS_PER_MONTH_EVEN;
			break;
		case 31:
			seconds += SECS_PER_MONTH_ODD;
			break;
		default:
			break;
		}
	}

	seconds += (pTm.DayOfMonth - 1) * SECS_PER_DAY;
	seconds += pTm.Hours * SECS_PER_HOUR;
	seconds += pTm.Minutes * SECS_PER_MIN;
	seconds += pTm.Seconds;

	return (tTime)seconds;
}

void RTCTimerClass::BreakTime(tTime &timeInput, sDateTime &tm)
{
	uint8_t year;
	uint8_t month, monthLength;
	uint32_t time;
	uint16_t days;

	/*sprintf(lBuffer, "RTC has been set %lu.", (uint32_t)RTCTimer.Time);
	SerialInterpreter.Send(lBuffer);*/

	time = (uint32_t)timeInput;
	tm.Seconds = time % 60;
	time /= 60; // now it is minutes
	tm.Minutes = time % 60;
	time /= 60; // now it is hours
	tm.Hours = time % 24;
	time /= 24; // now it is days
	tm.DayOfWeek = ((time + 4) % 7) + 1;  // Sunday is day 1 

	year = 0;
	days = 0;
	for (;;)
	{
		days += (LEAP_YEAR(year) ? 366 : 365);
		if (days < time)
			year++;
		else
			break;
	}
	tm.Year = year + 2000; // year is offset from 2000 

	days -= (LEAP_YEAR(year) ? 366 : 365);
	time -= days; // now it is days in this year, starting at 0

	days = 0;
	month = 0;
	monthLength = 0;

	for (month = 0; month < 12; month++)
	{
		if ((month == 1) && (LEAP_YEAR(year)))
		{
			monthLength = 29;
		}
		else
		{
			monthLength = monthDays[month];
		}

		if (time >= monthLength)
		{
			time -= monthLength;
		}
		else
		{
			break;
		}
	}

	tm.Month = month + 1;  // jan is month 1  
	tm.DayOfMonth = time + 1;     // day of month
}

RTCTimerClass RTCTimer;