#include "RTCTimer.h"

RTCTimerClass::RTCTimerClass()
{
	bind_member<RTCTimerClass, &RTCTimerClass::OnInterrupt> tCallBack(this);
	timer0_isr_init();
	timer0_attachInterrupt(tCallBack);
	timer0_write(ESP.getCycleCount() + CYCLE_COUNT);
}

void RTCTimerClass::OnInterrupt()
{
	timer0_write(ESP.getCycleCount() + CYCLE_COUNT);

	miliToSec++;
	MiliSeconds++;

	if (miliToSec >= 1000)
	{
		miliToSec = 0;
		Time++;
	}
}

void RTCTimerClass::DelayMili(uint32_t pMili)
{
	tmpTicks = MiliSeconds + pMili;

	while (MiliSeconds <= tmpTicks) { yield(); }
}

void RTCTimerClass::DelayMili(uint32_t pMili, bool &pFlag)
{
	tmpTicks = MiliSeconds + pMili;

	while ((!pFlag) && (MiliSeconds <= tmpTicks)) { yield(); }
}

void RTCTimerClass::DelayMili(uint32_t pMili, void(*doWhile)(void))
{
	tmpTicks = MiliSeconds + pMili;

	while (MiliSeconds <= tmpTicks)
	{
		doWhile();
	}
}

bool RTCTimerClass::DelayMili(uint32_t pMili, bool(*doWhile)(void))
{
	tmpTicks = MiliSeconds + pMili;

	while (MiliSeconds <= tmpTicks)
	{
		if (doWhile())
			return true;
	}

	return false;
}

void RTCTimerClass::DelayMili(uint32_t pMili, bool &pFlag, void(*doWhile)(void))
{
	tmpTicks = MiliSeconds + pMili;

	while ((!pFlag) && (MiliSeconds <= tmpTicks))
	{
		doWhile();
	}
}

void RTCTimerClass::SetTime(tTime pTime)
{
	Time = pTime;
}

RTCTimerClass RTCTimer;