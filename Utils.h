#pragma once

#ifndef _UTILS_h
#define _UTILS_h

#if defined(ESP8266)
#include <pgmspace.h>
#else
#include <avr/pgmspace.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define byte uint8_t

const PROGMEM char HEX_VALUES[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

class UtilsClass
{
public:
	static void HexToByteArray(byte * pBuffer, char * pString, uint16_t pStrLength);
    static void ByteToHexString(char * pBuffer, byte * pByteArray, uint16_t pByteArrayLength);
};

extern UtilsClass Utils;

#endif