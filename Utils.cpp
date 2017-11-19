#include "Utils.h"

void UtilsClass::HexToByteArray(byte * pBuffer, char * pString, uint16_t pStrLength)
{
	memset(pBuffer, 0x00, pStrLength / 2);

	for (uint16_t i = 0, u = 0; i < pStrLength; i++, u++)
	{
		if (pString[i] <= '9')
			pBuffer[u] += pString[i] - '0';
		else if (pString[i] <= 'F')
			pBuffer[u] += pString[i] - 'A' + 10;
		else if (pString[i] <= 'f')
			pBuffer[u] += pString[i] - 'a' + 10;

		pBuffer[u] = pBuffer[u] << 4;

		if (pString[++i] <= '9')
			pBuffer[u] += pString[i] - '0';
		else if (pString[i] <= 'F')
			pBuffer[u] += pString[i] - 'A' + 10;
		else if (pString[i] <= 'f')
			pBuffer[u] += pString[i] - 'a' + 10;
	}
}

void UtilsClass::ByteToHexString(char * pBuffer, byte * pByteArray, uint16_t pByteArrayLength)
{
	unsigned char num;

	memset(pBuffer, 0x00, (pByteArrayLength * 2) + 1);

	for (int i = 0, u = 0; i < pByteArrayLength; i++, u++)
	{
		num = pByteArray[i] >> 4;
		pBuffer[u] = (char)pgm_read_byte(HEX_VALUES + num);
		num = pByteArray[i] & 0xf;
		pBuffer[++u] = (char)pgm_read_byte(HEX_VALUES + num);
	}
}

UtilsClass Utils;