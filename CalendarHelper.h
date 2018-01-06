#pragma once

#ifndef CALENDAR_HELPER
#define CALENDAR_HELPER

#include <Arduino.h>

#define BASE_YEAR				2000UL
#define SECS_PER_MIN			60UL
#define SECS_PER_HOUR			3600UL
#define SECS_PER_DAY			SECS_PER_HOUR * 24UL
#define SECS_PER_YEAR			SECS_PER_DAY * 365UL
#define SECS_FEB				SECS_PER_DAY * 28UL
#define SECS_FEB_LEAP			SECS_PER_DAY * 29UL
#define SECS_PER_MONTH_EVEN		SECS_PER_DAY * 30UL
#define SECS_PER_MONTH_ODD		SECS_PER_DAY * 31UL

#define LEAP_YEAR(Y)	(((BASE_YEAR + Y) > 0) && !((BASE_YEAR + Y) % 4) && (((BASE_YEAR + Y) % 100) || !((BASE_YEAR + Y) % 400)))

const PROGMEM uint8_t MONTH_DAYS[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
const PROGMEM uint8_t SCHWERDTFEGER[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };

struct sDateTime
{
    uint16_t Year;
    uint8_t Month;
    uint8_t Day;
    uint8_t DayOfWeek;
    uint8_t Hour;
    uint8_t Minute;
    uint8_t Second;
};

class CalendarHelperClass
{
private:
    static void CarnavalSunday(sDateTime & pDateTime, uint16_t pYear);  // Return the Carnaval day from the given year

public:
    static void ParseStrDateTime(sDateTime & pDateTime, char pStrDateTime[]); // Parse from ISO 8601 string format to sDateTime
    static void ConvertToSeconds(uint32_t & pSeconds, sDateTime & pDateTime); // Convert from sDateTime to number of seconds since Jan 1st of 2000
    static void ConvertToDateTime(sDateTime & pDateTime, uint32_t pSeconds); // Convert from number of seconds since beginning of 2000 to sDateTime
    static uint8_t GetDayOfWeek(uint16_t pYear, uint8_t pMonth, uint8_t pDay); // Calculate day of week in proleptic Gregorian calendar. Sunday == 0

    static void BeginningOfSummerTime(sDateTime & pDateTime, uint16_t pYear); // Return the date when the summer time starts in Brazil
    static void EndingOfSummerTime(sDateTime & pDateTime, uint16_t pYear);
};

#endif

