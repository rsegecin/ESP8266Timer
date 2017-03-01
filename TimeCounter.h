#ifndef _RTCTIMER_h
#define _RTCTIMER_h

#include <stdint.h>
#include <Arduino.h>
#include "Communs.h"

extern "C" {
#include "user_interface.h"
}

#define DEBUG_PIN 16

class TimeCounterClass
{
public:
	struct sTimeSpan
	{
		volatile uint32_t Ticks;
		volatile uint16_t MiliSec;
		volatile uint8_t Seconds;
		volatile uint8_t Minutes;
		volatile uint8_t Hours;
		volatile uint16_t Days;
	};
	
	TimeCounterClass();

	volatile sTimeSpan TimeSpan;

	void DelayMili(uint16_t pMili);
	void DelayMili(uint8_t pMili, bool &pFlag);
	void DelayMili(uint8_t pMili, bool &pFlag, void (*doWhile)(void));
	void ResetTimer();

private:
	os_timer_t myTimer;
	uint32_t tmpTicks;
	void timerCallback();
	int stateLED = LOW;
};

extern TimeCounterClass TimeCounter;

#endif

