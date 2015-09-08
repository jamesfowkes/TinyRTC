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

bool ALARM_make(ALARM * alarm, INTERVAL interval, TM * alarm_time, uint16_t repeat)
{
	if (!alarm) { return false; }
	if (!alarm_time) { return false; }

	time_cpy(&alarm->datetime, alarm_time);
	alarm->repeat = repeat;
	return true;
} 