# ESP8266Timer

This is a timer library for Arduino ESP8266 using timer0.
Don't forget to change the value of timer0_write in TimeCounter.cpp accordingly with the clock you are running.
You can remove DEBUG_PIN, stateLED and everything related to it as well it's there for just debugging purposes to toggle the pin 16 every millisecond.
It uses timer0 in this library because it's more accurate than use os_timer_setfn noticed in this [post](https://github.com/esp8266/Arduino/issues/2426). I've checked with my oscilloscope and saw a consistent output.

Provided under GPL license.
