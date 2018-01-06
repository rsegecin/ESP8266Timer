#include "CalendarHelper.h"

void CalendarHelperClass::ParseStrDateTime(sDateTime & pDateTime, char pStrDateTime[])
{
    char aux[5];

    if ((pStrDateTime[10] == 'T') && ((pStrDateTime[19] == 'Z') || (pStrDateTime[23] == 'Z')))
    {
        memcpy(aux, pStrDateTime, 4);
        pDateTime.Year = atoi(aux);
        memset(aux, 0x00, 5);
        memcpy(aux, &pStrDateTime[5], 2);
        pDateTime.Month = atoi(aux);
        memcpy(aux, &pStrDateTime[8], 2);
        pDateTime.Day = atoi(aux);
        memcpy(aux, &pStrDateTime[11], 2);
        pDateTime.Hour = atoi(aux);
        memcpy(aux, &pStrDateTime[14], 2);
        pDateTime.Minute = atoi(aux);
        memcpy(aux, &pStrDateTime[17], 2);
        pDateTime.Second = atoi(aux);
        pDateTime.DayOfWeek = GetDayOfWeek(pDateTime.Year, pDateTime.Month, pDateTime.Day);
    }
}

void CalendarHelperClass::ParseStrDateTime(uint32_t & pTime, char pStrDateTime[])
{
    sDateTime datetime;
    CalendarHelperClass::ParseStrDateTime(datetime, pStrDateTime);
    CalendarHelperClass::ConvertToSeconds(pTime, datetime);
}

void CalendarHelperClass::ConvertToSeconds(uint32_t & pSeconds, sDateTime & pDateTime)
{
    int i;
    int16_t Year = pDateTime.Year - 2000;

    // seconds from 2000 till 1st jan 00:00:00 of the given year
    pSeconds = Year * SECS_PER_YEAR;
    for (i = 0; i < Year; i++)
    {
        if (LEAP_YEAR(i))
        {
            pSeconds += SECS_PER_DAY;
        }
    }

    // add days for this actual year passed on datetime, months start from 1
    for (i = 1; i < pDateTime.Month; i++)
    {
        switch (pgm_read_byte(MONTH_DAYS + i - 1))
        {
        case 28:
            if (LEAP_YEAR(Year))
                pSeconds += SECS_FEB_LEAP;
            else
                pSeconds += SECS_FEB;
            break;
        case 30:
            pSeconds += SECS_PER_MONTH_EVEN;
            break;
        case 31:
            pSeconds += SECS_PER_MONTH_ODD;
            break;
        default:
            break;
        }
    }

    pSeconds += (pDateTime.Day - 1) * SECS_PER_DAY;
    pSeconds += pDateTime.Hour * SECS_PER_HOUR;
    pSeconds += pDateTime.Minute * SECS_PER_MIN;
    pSeconds += pDateTime.Second;
}

void CalendarHelperClass::ConvertToDateTime(sDateTime & pDateTime, uint32_t pSeconds)
{
    uint8_t Year;
    uint8_t Month, monthLength;
    uint32_t time;
    uint16_t days;

    /*sprintf(lBuffer, "RTC has been set %lu.", (uint32_t)RTCTimer.Time);
    SerialInterpreter.Send(lBuffer);*/

    time = (uint32_t)pSeconds;
    pDateTime.Second = time % 60;
    time /= 60; // now it is minutes
    pDateTime.Minute = time % 60;
    time /= 60; // now it is hours
    pDateTime.Hour = time % 24;
    time /= 24; // now it is days
    pDateTime.DayOfWeek = ((time + 4) % 7) + 1;  // Sunday is day 1 

    Year = 0;
    days = 0;
    for (;;)
    {
        days += (LEAP_YEAR(Year) ? 366 : 365);
        if (days < time)
            Year++;
        else
            break;
    }
    pDateTime.Year = Year + 2000; // year is offset from 2000 

    days -= (LEAP_YEAR(Year) ? 366 : 365);
    time -= days; // now it is days in this year, starting at 0

    days = 0;
    Month = 0;
    monthLength = 0;

    for (Month = 0; Month < 12; Month++)
    {
        if ((Month == 1) && (LEAP_YEAR(Year)))
        {
            monthLength = 29;
        }
        else
        {
            monthLength = pgm_read_byte(MONTH_DAYS + Month);
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

    pDateTime.Month = Month + 1;  // jan is month 1  
    pDateTime.Day = time + 1;     // day of month
}

uint8_t CalendarHelperClass::GetDayOfWeek(uint16_t pYear, uint8_t pMonth, uint8_t pDay)
{
    int adjustment, mm, yy;

    adjustment = (14 - pMonth) / 12;
    mm = pMonth + 12 * adjustment - 2;
    yy = pYear - adjustment;
    return (pDay + (13 * mm - 1) / 5 + yy + yy / 4 - yy / 100 + yy / 400) % 7;
}

void CalendarHelperClass::CarnavalSunday(sDateTime & pDateTime, uint16_t pYear)
{
    uint8_t a = pYear % 19;
    uint8_t b = pYear / 100;
    uint8_t c = pYear % 100;
    uint8_t d = b / 4;
    uint8_t e = b % 4;
    uint8_t f = (b + 8) / 25;
    uint8_t g = (b - f + 1) / 3;
    uint8_t h = (19 * a + b - d - g + 15) % 30;
    uint8_t i = c / 4;
    uint8_t k = c % 4;
    uint8_t L = (32 + 2 * e + 2 * i - h - k) % 7;
    uint8_t m = (a + 11 * h + 22 * L) / 451;
    uint8_t Month = (h + L - 7 * m + 114) / 31;
    uint8_t Day = ((h + L - 7 * m + 114) % 31) + 1;

    pDateTime.Year = pYear;
    pDateTime.Month = Month;
    pDateTime.Day = Day;

    uint32_t time;
    CalendarHelperClass::ConvertToSeconds(time, pDateTime);
    time -= 49 * SECS_PER_DAY;
    CalendarHelperClass::ConvertToDateTime(pDateTime, time);
}

void CalendarHelperClass::BeginningOfSummerTime(sDateTime & pDateTime, uint16_t pYear)
{
    // Beginning of the summer time is on November's first sunday
    pDateTime.Year = pYear;
    pDateTime.Month = 11;
    pDateTime.Day = 8 - CalendarHelperClass::GetDayOfWeek(pYear, 11, 1); // sum 8 because dow starts at 0 and days at 1
}

void CalendarHelperClass::EndingOfSummerTime(sDateTime & pDateTime, uint16_t pYear)
{
    sDateTime carnavalSunday;

    CalendarHelperClass::CarnavalSunday(carnavalSunday, pYear);

    // Sets the third february sunday
    pDateTime.Year = pYear;
    pDateTime.Month = 2;
    pDateTime.Day = 14 + ((7 - CalendarHelperClass::GetDayOfWeek(pYear, 2, 15)) % 7);

    if (carnavalSunday.Day == pDateTime.Day)
        pDateTime.Day += 7;
}

uint32_t CalendarHelperClass::Difference(sDateTime & pDateTimeOne, sDateTime & pDateTimeTwo)
{
    uint32_t one;
    uint32_t two;

    CalendarHelperClass::ConvertToSeconds(one, pDateTimeOne);
    CalendarHelperClass::ConvertToSeconds(two, pDateTimeTwo);

    if (one > two)
        return one - two;
    else
        return two - one;
}

void CalendarHelperClass::SPrintTime(char * pBuffer, sDateTime & pDateTime)
{
    sprintf(pBuffer, "%i-%i-%i %i:%i:%i",
        pDateTime.Year, pDateTime.Month, pDateTime.Day, 
        pDateTime.Hour, pDateTime.Minute, pDateTime.Second);
}

