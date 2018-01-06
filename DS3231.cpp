/*
DS3231.h - Class file for the DS3231 Real-Time Clock

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

#include "DS3231.h"

bool DS3231Class::Begin(void)
{
    Wire.begin();

    SetBattery(true, false);

    return true;
}

void DS3231Class::SetDateTime(sDateTime & pDateTime)
{
    Wire.beginTransmission(DS3231_ADDRESS);

    WireWrite(DS3231_REG_TIME);

    WireWrite(dec2bcd(pDateTime.Second));
    WireWrite(dec2bcd(pDateTime.Minute));
    WireWrite(dec2bcd(pDateTime.Hour));
    WireWrite(dec2bcd(CalendarHelperClass::GetDayOfWeek(pDateTime.Year, pDateTime.Month, pDateTime.Day))); // 0 is sunday
    WireWrite(dec2bcd(pDateTime.Day));
    WireWrite(dec2bcd(pDateTime.Month));
    WireWrite(dec2bcd(pDateTime.Year - 2000));

    WireWrite(DS3231_REG_TIME);

    Wire.endTransmission();
}

void DS3231Class::GetDateTime(sDateTime & pDateTime)
{
    int values[7];

    Wire.beginTransmission(DS3231_ADDRESS);
    WireWrite(DS3231_REG_TIME);
    Wire.endTransmission();

    Wire.requestFrom(DS3231_ADDRESS, 7);

    while (!Wire.available()) {};

    for (uint8_t i = 0; i < 7; i++)
    {
        values[i] = bcd2dec(WireRead());
    }

    Wire.endTransmission();

    pDateTime.Second = values[0];
    pDateTime.Minute = values[1];
    pDateTime.Hour = values[2];
    pDateTime.DayOfWeek = values[3];
    pDateTime.Day = values[4];
    pDateTime.Month = values[5];
    pDateTime.Year = values[6] + 2000;
}

void DS3231Class::GetAlarm1(sAlarmTime & pAlarmTime)
{
    uint8_t values[4];

    Wire.beginTransmission(DS3231_ADDRESS);
    WireWrite(DS3231_REG_ALARM_1);
    Wire.endTransmission();

    Wire.requestFrom(DS3231_ADDRESS, 4);

    while (!Wire.available()) {};

    for (int i = 3; i >= 0; i--)
    {
        values[i] = bcd2dec(WireRead() & 0b01111111);
    }

    Wire.endTransmission();

    pAlarmTime.Day = values[0];
    pAlarmTime.Hour = values[1];
    pAlarmTime.Minute = values[2];
    pAlarmTime.Second = values[3];
}

void DS3231Class::GetAlarmType1(eDS3231_alarm1_t & pDS3231_alarm1_t)
{
    uint8_t values[4];
    uint8_t mode = 0;

    Wire.beginTransmission(DS3231_ADDRESS);
    WireWrite(DS3231_REG_ALARM_1);
    Wire.endTransmission();

    Wire.requestFrom(DS3231_ADDRESS, 4);

    while (!Wire.available()) {};

    for (int i = 3; i >= 0; i--)
    {
        values[i] = bcd2dec(WireRead());
    }

    Wire.endTransmission();

    mode |= ((values[3] & 0b01000000) >> 6);
    mode |= ((values[2] & 0b01000000) >> 5);
    mode |= ((values[1] & 0b01000000) >> 4);
    mode |= ((values[0] & 0b01000000) >> 3);
    mode |= ((values[0] & 0b00100000) >> 1);

    pDS3231_alarm1_t = (eDS3231_alarm1_t)mode;
}

void DS3231Class::SetAlarm1(uint8_t dydw, uint8_t Hour, uint8_t Minute, uint8_t Second, eDS3231_alarm1_t mode, bool armed)
{
    Second = dec2bcd(Second);
    Minute = dec2bcd(Minute);
    Hour = dec2bcd(Hour);
    dydw = dec2bcd(dydw);

    switch (mode)
    {
    case DS3231_EVERY_SECOND:
        Second |= 0b10000000;
        Minute |= 0b10000000;
        Hour |= 0b10000000;
        dydw |= 0b10000000;
        break;

    case DS3231_MATCH_S:
        Second &= 0b01111111;
        Minute |= 0b10000000;
        Hour |= 0b10000000;
        dydw |= 0b10000000;
        break;

    case DS3231_MATCH_M_S:
        Second &= 0b01111111;
        Minute &= 0b01111111;
        Hour |= 0b10000000;
        dydw |= 0b10000000;
        break;

    case DS3231_MATCH_H_M_S:
        Second &= 0b01111111;
        Minute &= 0b01111111;
        Hour &= 0b01111111;
        dydw |= 0b10000000;
        break;

    case DS3231_MATCH_DT_H_M_S:
        Second &= 0b01111111;
        Minute &= 0b01111111;
        Hour &= 0b01111111;
        dydw &= 0b01111111;
        break;

    case DS3231_MATCH_DY_H_M_S:
        Second &= 0b01111111;
        Minute &= 0b01111111;
        Hour &= 0b01111111;
        dydw &= 0b01111111;
        dydw |= 0b01000000;
        break;
    }

    Wire.beginTransmission(DS3231_ADDRESS);
    WireWrite(DS3231_REG_ALARM_1);
    WireWrite(Second);
    WireWrite(Minute);
    WireWrite(Hour);
    WireWrite(dydw);
    Wire.endTransmission();

    ArmAlarm1(armed);

    ClearAlarm1();
}

bool DS3231Class::IsAlarm1(bool clear)
{
    uint8_t alarm;

    alarm = readRegister8(DS3231_REG_STATUS);
    alarm &= 0b00000001;

    if (alarm && clear)
    {
        ClearAlarm1();
    }

    return alarm;
}

void DS3231Class::ArmAlarm1(bool armed)
{
    uint8_t value;
    value = readRegister8(DS3231_REG_CONTROL);

    if (armed)
    {
        value |= 0b00000001;
    }
    else
    {
        value &= 0b11111110;
    }

    writeRegister8(DS3231_REG_CONTROL, value);
}

bool DS3231Class::IsArmed1(void)
{
    uint8_t value;
    value = readRegister8(DS3231_REG_CONTROL);
    value &= 0b00000001;
    return value;
}

void DS3231Class::ClearAlarm1(void)
{
    uint8_t value;

    value = readRegister8(DS3231_REG_STATUS);
    value &= 0b11111110;

    writeRegister8(DS3231_REG_STATUS, value);
}

void DS3231Class::GetAlarm2(sAlarmTime & pAlarmTime)
{
    uint8_t values[3];

    Wire.beginTransmission(DS3231_ADDRESS);
    WireWrite(DS3231_REG_ALARM_2);
    Wire.endTransmission();

    Wire.requestFrom(DS3231_ADDRESS, 3);

    while (!Wire.available()) {};

    for (int i = 2; i >= 0; i--)
    {
        values[i] = bcd2dec(WireRead() & 0b01111111);
    }

    Wire.endTransmission();

    pAlarmTime.Day = values[0];
    pAlarmTime.Hour = values[1];
    pAlarmTime.Minute = values[2];
    pAlarmTime.Second = 0;
}

void DS3231Class::GetAlarmType2(eDS3231_alarm2_t & pDS3231_alarm2_t)
{
    uint8_t values[3];
    uint8_t mode = 0;

    Wire.beginTransmission(DS3231_ADDRESS);
    WireWrite(DS3231_REG_ALARM_2);
    Wire.endTransmission();

    Wire.requestFrom(DS3231_ADDRESS, 3);

    while (!Wire.available()) {};

    for (int i = 2; i >= 0; i--)
    {
        values[i] = bcd2dec(WireRead());
    }

    Wire.endTransmission();

    mode |= ((values[2] & 0b01000000) >> 5);
    mode |= ((values[1] & 0b01000000) >> 4);
    mode |= ((values[0] & 0b01000000) >> 3);
    mode |= ((values[0] & 0b00100000) >> 1);

    pDS3231_alarm2_t = (eDS3231_alarm2_t)mode;
}

void DS3231Class::SetAlarm2(uint8_t dydw, uint8_t Hour, uint8_t Minute, eDS3231_alarm2_t mode, bool armed)
{
    Minute = dec2bcd(Minute);
    Hour = dec2bcd(Hour);
    dydw = dec2bcd(dydw);

    switch (mode)
    {
    case DS3231_EVERY_MINUTE:
        Minute |= 0b10000000;
        Hour |= 0b10000000;
        dydw |= 0b10000000;
        break;

    case DS3231_MATCH_M:
        Minute &= 0b01111111;
        Hour |= 0b10000000;
        dydw |= 0b10000000;
        break;

    case DS3231_MATCH_H_M:
        Minute &= 0b01111111;
        Hour &= 0b01111111;
        dydw |= 0b10000000;
        break;

    case DS3231_MATCH_DT_H_M:
        Minute &= 0b01111111;
        Hour &= 0b01111111;
        dydw &= 0b01111111;
        break;

    case DS3231_MATCH_DY_H_M:
        Minute &= 0b01111111;
        Hour &= 0b01111111;
        dydw &= 0b01111111;
        dydw |= 0b01000000;
        break;
    }

    Wire.beginTransmission(DS3231_ADDRESS);
    WireWrite(DS3231_REG_ALARM_2);
    WireWrite(Minute);
    WireWrite(Hour);
    WireWrite(dydw);
    Wire.endTransmission();

    ArmAlarm2(armed);

    ClearAlarm2();
}

void DS3231Class::ArmAlarm2(bool armed)
{
    uint8_t value;
    value = readRegister8(DS3231_REG_CONTROL);

    if (armed)
    {
        value |= 0b00000010;
    }
    else
    {
        value &= 0b11111101;
    }

    writeRegister8(DS3231_REG_CONTROL, value);
}

bool DS3231Class::IsArmed2(void)
{
    uint8_t value;
    value = readRegister8(DS3231_REG_CONTROL);
    value &= 0b00000010;
    value >>= 1;
    return value;
}

void DS3231Class::ClearAlarm2(void)
{
    uint8_t value;

    value = readRegister8(DS3231_REG_STATUS);
    value &= 0b11111101;

    writeRegister8(DS3231_REG_STATUS, value);
}

bool DS3231Class::IsAlarm2(bool clear)
{
    uint8_t alarm;

    alarm = readRegister8(DS3231_REG_STATUS);
    alarm &= 0b00000010;

    if (alarm && clear)
    {
        ClearAlarm2();
    }

    return alarm;
}

void DS3231Class::GetOutput(eDS3231_sqw_t &pMode)
{
    uint8_t value;

    value = readRegister8(DS3231_REG_CONTROL);

    value &= 0b00011000;
    value >>= 3;

    pMode = (eDS3231_sqw_t)value;
}

void DS3231Class::SetOutput(eDS3231_sqw_t mode)
{
    uint8_t value;

    value = readRegister8(DS3231_REG_CONTROL);

    value &= 0b11100111;
    value |= (mode << 3);

    writeRegister8(DS3231_REG_CONTROL, value);
}

void DS3231Class::EnableOutput(bool enabled)
{
    uint8_t value;

    value = readRegister8(DS3231_REG_CONTROL);

    value &= 0b11111011;
    value |= (!enabled << 2);

    writeRegister8(DS3231_REG_CONTROL, value);
}

bool DS3231Class::IsOutput(void)
{
    uint8_t value;

    value = readRegister8(DS3231_REG_CONTROL);

    value &= 0b00000100;
    value >>= 2;

    return !value;
}

void DS3231Class::Enable32kHz(bool enabled)
{
    uint8_t value;

    value = readRegister8(DS3231_REG_STATUS);

    value &= 0b11110111;
    value |= (enabled << 3);

    writeRegister8(DS3231_REG_STATUS, value);
}

bool DS3231Class::Is32kHz(void)
{
    uint8_t value;

    value = readRegister8(DS3231_REG_STATUS);

    value &= 0b00001000;
    value >>= 3;

    return value;
}

void DS3231Class::ForceConversion(void)
{
    uint8_t busy;
    uint8_t value;

    busy = readRegister8(DS3231_REG_STATUS);

    if (busy & 0b00000100)
    {
        value = readRegister8(DS3231_REG_CONTROL);
        value |= 0b00100000;
        writeRegister8(DS3231_REG_CONTROL, value);

        do {} while ((readRegister8(DS3231_REG_CONTROL) & 0b00100000) != 0);
    }
}

float DS3231Class::GetTemperature(void)
{
    uint8_t msb, lsb;

    Wire.beginTransmission(DS3231_ADDRESS);
    WireWrite(DS3231_REG_TEMPERATURE);
    Wire.endTransmission();

    Wire.requestFrom(DS3231_ADDRESS, 2);

    while (!Wire.available()) {};

    msb = WireRead();
    lsb = WireRead();

    return ((((short)msb << 8) | (short)lsb) >> 6) / 4.0f;
}

void DS3231Class::SetBattery(bool timeBattery, bool squareBattery)
{
    uint8_t value;

    value = readRegister8(DS3231_REG_CONTROL);

    if (squareBattery)
    {
        value |= 0b01000000;
    }
    else
    {
        value &= 0b10111111;
    }

    if (timeBattery)
    {
        value &= 0b01111011;
    }
    else
    {
        value |= 0b10000000;
    }

    writeRegister8(DS3231_REG_CONTROL, value);
}

uint8_t DS3231Class::bcd2dec(uint8_t bcd)
{
    return ((bcd / 16) * 10) + (bcd % 16);
}

uint8_t DS3231Class::dec2bcd(uint8_t dec)
{
    return ((dec / 10) * 16) + (dec % 10);
}

void DS3231Class::writeRegister8(uint8_t reg, uint8_t value)
{
    Wire.beginTransmission(DS3231_ADDRESS);
    WireWrite(reg);
    WireWrite(value);
    Wire.endTransmission();
}

uint8_t DS3231Class::readRegister8(uint8_t reg)
{
    uint8_t value;

    Wire.beginTransmission(DS3231_ADDRESS);
    WireWrite(reg);
    Wire.endTransmission();

    Wire.requestFrom(DS3231_ADDRESS, 1);
    while (!Wire.available()) {};
    value = WireRead();
    Wire.endTransmission();

    return value;
}

DS3231Class DS3231;