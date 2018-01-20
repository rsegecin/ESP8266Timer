#include "ESPTimer0.h"

ESPTimer0Class::ESPTimer0Class()
{
	bind_member<ESPTimer0Class, &ESPTimer0Class::OnInterrupt> tCallBack(this);
	timer0_isr_init();
	timer0_attachInterrupt(tCallBack);
	timer0_write(ESP.getCycleCount() + CYCLE_COUNT);
}

void ESPTimer0Class::OnInterrupt()
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

void ESPTimer0Class::DelayMili(uint32_t pMili)
{
    uint32_t tmpTicks = MiliSeconds + pMili;

	while (MiliSeconds <= tmpTicks) { yield(); }
}

void ESPTimer0Class::DelayMili(uint32_t pMili, bool &pFlag)
{
    uint32_t tmpTicks = MiliSeconds + pMili;

	while ((!pFlag) && (MiliSeconds <= tmpTicks)) { yield(); }
}

void ESPTimer0Class::DelayMili(uint32_t pMili, void(*doWhile)(void))
{
    uint32_t tmpTicks = MiliSeconds + pMili;

	while (MiliSeconds <= tmpTicks)
	{
		doWhile();
	}
}

bool ESPTimer0Class::DelayMili(uint32_t pMili, bool(*doWhile)(void))
{
    uint32_t tmpTicks = MiliSeconds + pMili;

	while (MiliSeconds <= tmpTicks)
	{
		if (doWhile())
			return true;
	}

	return false;
}

void ESPTimer0Class::DelayMili(uint32_t pMili, bool &pFlag, void(*doWhile)(void))
{
    uint32_t tmpTicks = MiliSeconds + pMili;

	while ((!pFlag) && (MiliSeconds <= tmpTicks))
	{
		doWhile();
	}
}

void ESPTimer0Class::SetTime(tTime pTime)
{
	Time = pTime;
}