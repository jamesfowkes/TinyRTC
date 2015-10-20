/*
 * C Library Includes
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/*
 * Code Library Includes
 */

#include "Utility/util_time.h"

/*
 * Application Includes
 */

#include "app.rtc.h"
 
void app_get_rtc_datetime(TM * tm)
{
	tm->tm_sec = 23;
	tm->tm_min = 42;
	tm->tm_hour = 17;
	tm->tm_mday = 21;
	tm->tm_mon = 4; // This is MAY, not April! (0 - 11 months)
	tm->tm_year = 13;
	tm->tm_wday = 2; // Tuesday
}
