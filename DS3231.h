/*
DS3231.h - Header file for the DS3231 Real-Time Clock

Version: 2.0.0
(c) 2014 Korneliusz Jarzebski - www.jarzebski.pl
(c) 2018 Rinaldi Segecin

This program is free software: you can redistribute it and/or modify
it under the terms of the version 3 GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#ifndef _DS3231_h
#define _DS3231_h

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "CalendarHelper.h"
#include <Wire.h>

#define DS3231_ADDRESS              (0x68)

#define DS3231_REG_TIME             (0x00)
#define DS3231_REG_ALARM_1          (0x07)
#define DS3231_REG_ALARM_2          (0x0B)
#define DS3231_REG_CONTROL          (0x0E)
#define DS3231_REG_STATUS           (0x0F)
#define DS3231_REG_TEMPERATURE      (0x11)

struct sAlarmTime
{
    uint8_t Day;
    uint8_t Hour;
    uint8_t Minute;
    uint8_t Second;
};

typedef enum
{
    DS3231_1HZ      = 0x00,
    DS3231_4096HZ   = 0x01,
    DS3231_8192HZ   = 0x02,
    DS3231_32768HZ  = 0x03
} eDS3231_sqw_t;

typedef enum
{
    DS3231_EVERY_SECOND     = 0b00001111,
    DS3231_MATCH_S          = 0b00001110,
    DS3231_MATCH_M_S        = 0b00001100,
    DS3231_MATCH_H_M_S      = 0b00001000,
    DS3231_MATCH_DT_H_M_S   = 0b00000000,
    DS3231_MATCH_DY_H_M_S   = 0b00010000
} eDS3231_alarm1_t;

typedef enum
{
    DS3231_EVERY_MINUTE = 0b00001110,
    DS3231_MATCH_M      = 0b00001100,
    DS3231_MATCH_H_M    = 0b00001000,
    DS3231_MATCH_DT_H_M = 0b00000000,
    DS3231_MATCH_DY_H_M = 0b00010000
} eDS3231_alarm2_t;

class DS3231Class
{
public:
    bool Begin(void);

    void SetDateTime(sDateTime & pDateTime); // Set the RTC's module to the given pDateTime
    void GetDateTime(sDateTime & pDateTime); // Get the RTC's module to the given pDateTime

    void SetAlarm1(uint8_t dydw, uint8_t Hour, uint8_t Minute, uint8_t Second, eDS3231_alarm1_t mode, bool armed = true);
    void GetAlarm1(sAlarmTime & pAlarmTime);
    void GetAlarmType1(eDS3231_alarm1_t & pDS3231_alarm1_t);
    bool IsAlarm1(bool clear = true);
    void ArmAlarm1(bool armed);
    bool IsArmed1(void);
    void ClearAlarm1(void);

    void SetAlarm2(uint8_t dydw, uint8_t Hour, uint8_t Minute, eDS3231_alarm2_t mode, bool armed = true);
    void GetAlarm2(sAlarmTime & pAlarmTime);
    void GetAlarmType2(eDS3231_alarm2_t & pDS3231_alarm2_t);
    bool IsAlarm2(bool clear = true);
    void ArmAlarm2(bool armed);
    bool IsArmed2(void);
    void ClearAlarm2(void);

    void GetOutput(eDS3231_sqw_t &pMode);
    void SetOutput(eDS3231_sqw_t pMode);
    void EnableOutput(bool enabled);
    bool IsOutput(void);
    void Enable32kHz(bool enabled);
    bool Is32kHz(void);

    void ForceConversion(void);
    float GetTemperature(void);

    void SetBattery(bool timeBattery, bool squareBattery);

private:
    inline uint8_t WireRead() {
#if ARDUINO >= 100
        return Wire.read();
#else
        return Wire.receive();
#endif
    };

    inline void WireWrite(uint8_t data) {
#if ARDUINO >= 100
        Wire.write(data);
#else
        Wire.send(data);
#endif
    };

    static uint8_t bcd2dec(uint8_t bcd);
    static uint8_t dec2bcd(uint8_t dec);

    void writeRegister8(uint8_t reg, uint8_t value);
    uint8_t readRegister8(uint8_t reg);
};

#endif