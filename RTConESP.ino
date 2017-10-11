#include "SerialInterpreter.h"
#include "RTCTimer.h"

char lBuffer[DEF_MSG_SIZE];

#define NUMBER_OF_COMMANDS	3
sSerialCommand SerialCommands[NUMBER_OF_COMMANDS];
SerialInterpreterClass SerialInterpreter(SerialCommands, NUMBER_OF_COMMANDS);

void setup()
{
	sei();
	Serial.begin(115200);

	Serial.println("Welcome");

	SerialCommands[0].Name = "setdate";
	SerialCommands[0].ExecFunction = SetDateTime;

	SerialCommands[1].Name = "parsedate";
	SerialCommands[1].ExecFunction = ParseDate;

	SerialCommands[2].Name = "print";
	SerialCommands[2].ExecFunction = PrintTime;
}

void loop()
{
	RTCTimer.DelayMili(1000, (bool &)SerialInterpreter.MessageReady, CheckingSerial);

	PrintTime();

	if (SerialInterpreter.MessageReady)
	{
		if (SerialInterpreter.ExecFunction != nullptr)
			SerialInterpreter.ExecFunction();
		SerialInterpreter.ClearBuffer();
	}
}

void CheckingSerial()
{
	if (Serial.available())
	{
		SerialInterpreter.OnInterrupt(Serial.read());
	}

	yield();
}

void SetDateTime()
{
	sprintf(lBuffer, "Setting.");
	RTCTimer.SetTime(SerialInterpreter.GetParameter(0));
	Serial.println(lBuffer);
	PrintDateTime(RTCTimer.DateTime);
}

void ParseDate()
{
	static sDateTime datetime;
	RTCTimer.ParseTime(datetime, SerialInterpreter.GetParameter(0));
	sprintf(lBuffer, "Parsing.");
	Serial.println(lBuffer);
	PrintDateTime(datetime);
}

void PrintDateTime(sDateTime datetime)
{
	sprintf(lBuffer, "printing: %i/%i/%i %i:%i:%i",
		datetime.DayOfMonth, datetime.Month, datetime.Year,
		datetime.Hours, datetime.Minutes, datetime.Seconds);
	Serial.println(lBuffer);
}

void PrintTime()
{
	sDateTime conv;
	RTCTimer.BreakTime(RTCTimer.Time, conv);

	sprintf(lBuffer, "now: %i/%i/%i %i:%i:%i",
		conv.DayOfMonth, conv.Month, conv.Year, conv.Hours, conv.Minutes, conv.Seconds);
	Serial.println(lBuffer);
}