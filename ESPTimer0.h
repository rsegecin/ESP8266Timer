#pragma once

#ifndef _RTCTIMER_h
#define _RTCTIMER_h

#include <stdint.h>
#include <Arduino.h>
#include "CalendarHelper.h"
#include "SimpleBinders.h"

extern "C" {
#include "user_interface.h"
}

typedef uint32_t tTime;

#ifdef MHZ_160
#define CYCLE_COUNT 80000
#else
#define CYCLE_COUNT 160000
#endif

class ESPTimer0Class
{
public:
	ESPTimer0Class();

	uint32_t Time;
	sDateTime DateTime;
	volatile uint32_t MiliSeconds;
	void DelayMili(uint32_t pMili);
	void DelayMili(uint32_t pMili, bool &pFlag);
	void DelayMili(uint32_t pMili, void(*doWhile)(void));
	bool DelayMili(uint32_t pMili, bool(*doWhile)(void));
	void DelayMili(uint32_t pMili, bool &pFlag, void(*doWhile)(void));
	void SetTime(tTime pTime);

private:
	void OnInterrupt();
	volatile uint32_t miliToSec;
};

#endif
