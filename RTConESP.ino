#define MHZ_160

#include "CalendarHelper.h"
#include "SerialInterpreter.h"
#include "RTCTimer.h"
#include "MD5pm.h"
#include "Utils.h"

char serialBuffer[DEF_MSG_SIZE];

#define NUMBER_OF_COMMANDS	5
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

	SerialCommands[3].Name = "md5";
	SerialCommands[3].ExecFunction = PrintMD5;

	SerialCommands[4].Name = "hex2byte";
	SerialCommands[4].ExecFunction = HexToByte;
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
    sDateTime d;
    uint32_t t;
    CalendarHelperClass::ParseStrDateTime(d, SerialInterpreter.GetParameter(0));
    CalendarHelperClass::ConvertToSeconds(t, d);
	sprintf(serialBuffer, "Setting.");
	RTCTimer.SetTime(t);
	Serial.println(serialBuffer);
	PrintDateTime(RTCTimer.DateTime);
}

void ParseDate()
{
	static sDateTime datetime;
	CalendarHelperClass::ParseStrDateTime(datetime, SerialInterpreter.GetParameter(0));
	sprintf(serialBuffer, "Parsing.");
	Serial.println(serialBuffer);
	PrintDateTime(datetime);
}

void PrintDateTime(sDateTime datetime)
{
	sprintf(serialBuffer, "printing: %i/%i/%i %i:%i:%i",
		datetime.Day, datetime.Month, datetime.Year,
		datetime.Hour, datetime.Minute, datetime.Second);
	Serial.println(serialBuffer);
}

void PrintTime()
{
	sDateTime conv;
	CalendarHelperClass::ConvertToDateTime(conv, RTCTimer.Time);

	sprintf(serialBuffer, "now: %i/%i/%i %i:%i:%i",
		conv.Day, conv.Month, conv.Year, conv.Hour, conv.Minute, conv.Second);
	Serial.println(serialBuffer);
}

void PrintMD5() {
	char Hash[33];

	MD5pm.MakeHash(Hash, SerialInterpreter.GetParameter(0));

	sprintf(serialBuffer, "Hash of %s is %s", SerialInterpreter.GetParameter(0), Hash);
	Serial.println(serialBuffer);
}

void HexToByte() {
	byte buffer[DEF_MSG_SIZE / 2];
	char aux[DEF_MSG_SIZE];

	uint8_t length = strlen(SerialInterpreter.GetParameter(0));
	
	Utils.HexToByteArray(buffer, SerialInterpreter.GetParameter(0), length);
	Utils.ByteToHexString(aux, buffer, length / 2);

	sprintf(serialBuffer, "Byte array is %s", aux);
	Serial.println(serialBuffer);
}