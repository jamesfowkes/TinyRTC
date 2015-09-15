/*
 * C Library Includes
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#ifdef TEST
#include <cppunit/TestAssert.h>
#endif

/*
 * Code Library Includes
 */

#include "Utility/util_time.h"

/*
 * Application Includes
 */

#include "alarm.h"

/*
 * Public Functions
 */

void set_default_alarm_time(TM* tm)
{
	// Reset to 1st January 2000
	tm->tm_year = 100;
	tm->tm_mon = JAN;
	tm->tm_mday = 1;
	tm->tm_wday = SAT;
	tm->tm_yday = 0;
	tm->tm_hour = 0;
	tm->tm_min = 0;
	tm->tm_sec = 0;
}

/*
 * Alarm Class
 */

Alarm::Alarm()
{
	reset();
}

Alarm::Alarm(INTERVAL interval, TM * alarm_time, int repeat, int duration)
{
	if (!alarm_time) { return; }

	time_cpy(&m_datetime, alarm_time);
	m_repeat = repeat;
	m_repeat_interval = interval;

	m_duration = duration ? duration : 60;
    m_triggered = false;

    m_valid = true;
}

void Alarm::reset()
{
	set_default_alarm_time(&m_datetime);
	m_repeat = 1;
	m_repeat_interval = INTERVAL_YEAR;
	m_duration = 60;

    m_triggered = false;
}

bool operator==(const Alarm& lhs, const Alarm& rhs)
{
	bool equal = true;
	//equal &= times_equal(&lhs.m_datetime, &rhs.m_datetime);

	//equal &= (lhs.m_datetime.tm_sec == rhs.m_datetime.tm_sec);
	//equal &= (lhs.m_datetime.tm_min == rhs.m_datetime.tm_min);
	//equal &= (lhs.m_datetime.tm_hour == rhs.m_datetime.tm_hour);
	//equal &= (lhs.m_datetime.tm_mday == rhs.m_datetime.tm_mday);
	//equal &= (lhs.m_datetime.tm_mon == rhs.m_datetime.tm_mon);
	equal &= (lhs.m_datetime.tm_year == rhs.m_datetime.tm_year);
	equal &= (lhs.m_datetime.tm_wday == rhs.m_datetime.tm_wday);
	equal &= (lhs.m_datetime.tm_yday == rhs.m_datetime.tm_yday);

	equal &= (lhs.m_repeat == rhs.m_repeat);
	equal &= (lhs.m_repeat_interval == rhs.m_repeat_interval);
	equal &= (lhs.m_duration == rhs.m_duration);

	return equal;
}

bool Alarm::to_string(ALARM_STRING * str) const
{
	if (!time_to_datetime_string(&m_datetime, &str->datetime)) { return false; }

	str->space1 = ' ';
	str->r = 'r';
	str->repeat[1] = (m_repeat % 10) + '0';
	str->repeat[0] = (m_repeat / 10) + '0';
	str->space2 = ' ';
	str->i = 'i';
	str->interval= m_repeat_interval;
	str->space3 = ' ';
	str->d = 'd';
	str->null = '\0';

	int duration = m_duration;
	char c;

	c = duration / 10000; str->duration[0] = c + '0'; duration -= (c * 10000);
	c = duration / 1000; str->duration[1] = c + '0'; duration -= (c * 1000);
	c = duration / 100; str->duration[2] = c + '0'; duration -= (c * 100);
	c = duration / 10; str->duration[3] = c + '0'; duration -= (c * 10);
	str->duration[4] = duration + '0';
	
	return true;
}
