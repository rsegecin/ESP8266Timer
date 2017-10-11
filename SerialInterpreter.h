#ifndef SerialInterpreter_h
#define SerialInterpreter_h

#define SERIAL_TX		PD1
#define BAUD			19200
#define DEF_MSG_SIZE	80

#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include "Communs.h"

class SerialInterpreterClass
{
public:
	enum eSerialCommands
	{
		nSetDate, nParseDate, nPrint, nNenhum
	};

	SerialInterpreterClass();
	~SerialInterpreterClass();

	volatile bool MessageReady = false;   // Serial message's flag
	volatile eSerialCommands MessageCommand = nNenhum;   // Tells what command should be executed
	char StrParameters[DEF_MSG_SIZE];
	char MessageBuffer[DEF_MSG_SIZE];

	void ClearBuffer(void);
	char *GetParameter(unsigned char index);
	void AddCommand(char * strCmdParam, eSerialCommands nCmdParam);
	void OnInterrupt(char charParam);

private:
	char * usart_commands[nNenhum];
};

extern SerialInterpreterClass SerialInterpreter;

#endif