#include "SerialInterpreter.h"
#include "RTCTimer.h"

char lBuffer[DEF_MSG_SIZE];

void setup()
{
	sei();
	Serial.begin(115200);

	Serial.println("Welcome");

	SerialInterpreter.AddCommand("setdate", SerialInterpreterClass::eSerialCommands::nSetDate);
	SerialInterpreter.AddCommand("parsedate", SerialInterpreterClass::eSerialCommands::nParseDate);
	SerialInterpreter.AddCommand("print", SerialInterpreterClass::eSerialCommands::nPrint);
}

void CheckingSerial()
{
	if (Serial.available())
	{
		SerialInterpreter.OnInterrupt(Serial.read());
	}

	yield();
}

void loop()
{
	RTCTimer.DelayMili(1000, (bool &)SerialInterpreter.MessageReady, CheckingSerial);

	PrintTime();

	if (SerialInterpreter.MessageReady)
	{
		SerialHandler();
	}
}

void SerialHandler()
{
	switch (SerialInterpreter.MessageCommand)
	{
	case SerialInterpreterClass::eSerialCommands::nSetDate:
	{
		SetDateTime();
		SerialInterpreter.ClearBuffer();
		break;
	}
	case SerialInterpreterClass::eSerialCommands::nParseDate:
	{
		ParseDate();
		SerialInterpreter.ClearBuffer();
		break;
	}
	case SerialInterpreterClass::eSerialCommands::nPrint:
	{
		PrintTime();
		SerialInterpreter.ClearBuffer();
		break;
	}
	default:
		break;
	}
}

void SetDateTime()
{
	sprintf(lBuffer, "Setting.");
	RTCTimer.SetTime(SerialInterpreter.GetParameter(0));
	Serial.println(lBuffer);
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