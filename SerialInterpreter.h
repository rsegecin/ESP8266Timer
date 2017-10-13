#ifndef SerialInterpreter_h
#define SerialInterpreter_h

#define BAUD			19200
#define DEF_MSG_SIZE	80

#include <string.h>
#include <stdint.h>
#include <ctype.h>

struct sSerialCommand {
	char * Name;
	void(*ExecFunction)(void) = nullptr;
};

class SerialInterpreterClass
{
public:
	SerialInterpreterClass(sSerialCommand * pSerialCommands, int pNumberOfCommands);

	volatile bool MessageReady = false;		// Serial message's flag
	void(*ExecFunction)(void) = nullptr;	// Tells what command should be executed
	char StrParameters[DEF_MSG_SIZE];
	char MessageBuffer[DEF_MSG_SIZE];

	void ClearBuffer(void);
	char *GetParameter(unsigned char index);
	void OnInterrupt(char charParam);

private:
	sSerialCommand * SerialCommands = nullptr;
	int NumberOfCommands = 0;
};

#endif