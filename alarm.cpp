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
 * Private Functions
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
	m_datetime.tm_year = 0;
	m_datetime.tm_mon = JAN;
	m_datetime.tm_mday = 1;
	m_datetime.tm_wday = THU;
	m_datetime.tm_yday = 0;
	m_datetime.tm_hour = 0;
	m_datetime.tm_min = 0;
	m_datetime.tm_sec = 0;

	m_repeat = 1;
	m_repeat_interval = INTERVAL_YEAR;
	m_duration = 60;

    m_triggered = false;
}

bool operator==(const Alarm& lhs, const Alarm& rhs)
{
	bool equal = true;
	equal &= times_equal(&lhs.m_datetime, &rhs.m_datetime);

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

	int duration = m_duration;
	char c;

	c = duration / 10000; str->duration[0] = c + '0'; duration -= (c * 10000);
	c = duration / 1000; str->duration[1] = c + '0'; duration -= (c * 1000);
	c = duration / 100; str->duration[2] = c + '0'; duration -= (c * 100);
	c = duration / 10; str->duration[3] = c + '0'; duration -= (c * 10);
	str->duration[4] = duration + '0';
	
	return true;
}