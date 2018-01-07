#define MHZ_160

#include "DS3231.h"
#include "CalendarHelper.h"
#include "SerialInterpreter.h"
#include "ESPTimer0.h"
#include "MD5pm.h"
#include "Utils.h"

char serialBuffer[DEF_MSG_SIZE];

#define NUMBER_OF_COMMANDS	6
sSerialCommand SerialCommands[NUMBER_OF_COMMANDS];
SerialInterpreterClass SerialInterpreter(SerialCommands, NUMBER_OF_COMMANDS);

DS3231Class DS3231;
ESPTimer0Class ESPTimer0;

void setup()
{
	sei();
	Serial.begin(115200);

    DS3231.Begin();

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

    SerialCommands[5].Name = "setRTC";
    SerialCommands[5].ExecFunction = SetRTC;
}

void loop()
{
	ESPTimer0.DelayMili(1000, (bool &)SerialInterpreter.MessageReady, CheckingSerial);

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
	ESPTimer0.SetTime(t);
	Serial.println(serialBuffer);
	PrintDateTime(ESPTimer0.DateTime);
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
	CalendarHelperClass::ConvertToDateTime(conv, ESPTimer0.Time);

	sprintf(serialBuffer, "now: %i/%i/%i %i:%i:%i",
		conv.Day, conv.Month, conv.Year, conv.Hour, conv.Minute, conv.Second);
	Serial.println(serialBuffer);
}

void PrintMD5() {
    byte Hash[16];
    char strHash[33];

	MD5pm.MakeMD5((uint8_t *) SerialInterpreter.GetParameter(0), strlen(SerialInterpreter.GetParameter(0)), Hash);
    UtilsClass::ByteToHexString(strHash, Hash, sizeof(Hash));

	sprintf(serialBuffer, "Hash of %s is %s", SerialInterpreter.GetParameter(0), strHash);
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

void SetRTC()
{
    char auxBuffer[20];
    sDateTime datetime;
    CalendarHelperClass::ParseStrDateTime(datetime, SerialInterpreter.GetParameter(0));
    DS3231.SetDateTime(datetime);
    ESPTimer0.DelayMili(10);
    DS3231.GetDateTime(datetime);
    CalendarHelperClass::SPrintTime(auxBuffer, datetime);
    sprintf(serialBuffer, "Now the RTC date time is %s", auxBuffer);
    Serial.println(serialBuffer);
}