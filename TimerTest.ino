#include "TimeCounter.h"

char lBuffer[200];

void setup()
{
	Serial.begin(115200);
	Serial.println();
	Serial.println();
	Serial.println("--------------------------");
	Serial.println("ESP8266 Timer Test");
	Serial.println("--------------------------");
}

void loop()
{
	TimeCounter.DelayMili(1000);
	sprintf(lBuffer, "Timer %02d:%02d:%02d", TimeCounter.TimeSpan.Hours, TimeCounter.TimeSpan.Minutes, TimeCounter.TimeSpan.Seconds);
	Serial.println(lBuffer);
}