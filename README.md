SA:MP Plugin - Timerfix
=======================

Timerfix provides an improvement to the timers system existent in SA:MP server, increasing their accuracy.

Additional natives were introduced:

*	`SetTimer_(func[], interval, delay, count)`

	An improved version of SetTimer.
	
*	`SetTimerEx_(func[], interval, delay, count, format[], {Float, _}:...)`

	An improved version of SetTimerEx.
	
In order to enable this natives you must include the "timrefix.inc" file (`#include <timerfix>`). For more information, please check `timerfix.inc`.