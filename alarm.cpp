/*
 * C Library Includes
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

/*
 * Code Library Includes
 */

#include "Utility/util_time.h"

/*
 * Application Includes
 */

#include "alarm.h"

/*
 * Private Functions
 */

bool alarm_make(ALARM * alarm, INTERVAL interval, TM * alarm_time, int repeat, int duration)
{
	if (!alarm) { return false; }
	if (!alarm_time) { return false; }

	time_cpy(&alarm->datetime, alarm_time);
	alarm->repeat = repeat;
	alarm->repeat_interval = interval;

	alarm->duration = duration ? duration : 60;
	
	return true;
} 