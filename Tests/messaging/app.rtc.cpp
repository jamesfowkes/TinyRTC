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
 
void APP_GetRTCDatetime(TM * tm)
{
	tm->tm_sec = 23;
	tm->tm_min = 42;
	tm->tm_hour = 17;
	tm->tm_mday = 21;
	tm->tm_mon = 4;
	tm->tm_year = 13;
}