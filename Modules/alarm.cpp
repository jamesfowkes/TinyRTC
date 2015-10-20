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

Alarm::Alarm(INTERVAL interval, TM const * const alarm_time, int repeat, int duration)
{
	if (!alarm_time) { return; }

	time_cpy(&m_datetime, alarm_time);
	m_alarm_interval_count = repeat;
	m_alarm_interval_period = interval;
	m_current_trigger_count = 0;
	m_duration = duration ? duration : 60;
    m_triggered = false;

    m_valid = true;
}

void Alarm::update_deactivate_time(TM const * const current_time)
{
	m_deactivate_time_seconds = time_to_unix_seconds(current_time) + (m_duration * 60);
}

bool Alarm::set_current_time(TM const * const current_time)
{
	if (!current_time) { return false; }

	if (!m_triggered)
	{
		bool times_match = true;

		switch (m_alarm_interval_period)
		{
		case INTERVAL_YEAR:
			times_match &= (m_datetime.tm_mon == current_time->tm_mon);
			// Deliberate fall-through!
		case INTERVAL_MONTH:
			times_match &= (m_datetime.tm_mday == current_time->tm_mday);
			// Deliberate fall-through!
		case INTERVAL_DAY:
			times_match &= (m_datetime.tm_hour == current_time->tm_hour);
			// Deliberate fall-through!
		case INTERVAL_HOUR:
			times_match &= (m_datetime.tm_min == current_time->tm_min);
			break;
		case INTERVAL_WEEK:
			// Weekly interval is a special case
			times_match &= (m_datetime.tm_wday == current_time->tm_wday);
			times_match &= (m_datetime.tm_hour == current_time->tm_hour);
			times_match &= (m_datetime.tm_min == current_time->tm_min);
			break;
		}

		if (times_match) { m_current_trigger_count++; }

		m_triggered = (m_current_trigger_count == m_alarm_interval_count);

		if (m_triggered)
		{
			m_current_trigger_count = 0;
			update_deactivate_time(current_time);
		}
	}
	else
	{
		UNIX_TIMESTAMP current_time_seconds = time_to_unix_seconds(current_time);
		m_triggered = current_time_seconds <= m_deactivate_time_seconds;
	}

	return m_triggered;
}

void Alarm::reset()
{
	set_default_alarm_time(&m_datetime);
	m_alarm_interval_count = 1;
	m_alarm_interval_period = INTERVAL_YEAR;
	m_duration = 60;
	m_current_trigger_count = 0;
    m_triggered = false;
}

bool operator==(const Alarm& lhs, const Alarm& rhs)
{
	bool equal = true;
	equal &= times_equal(&lhs.m_datetime, &rhs.m_datetime);

	equal &= (lhs.m_alarm_interval_count == rhs.m_alarm_interval_count);
	equal &= (lhs.m_alarm_interval_period == rhs.m_alarm_interval_period);
	equal &= (lhs.m_duration == rhs.m_duration);

	return equal;
}

bool Alarm::to_string(ALARM_STRING * str) const
{
	if (!time_to_datetime_string(&m_datetime, &str->datetime)) { return false; }

	str->space1 = ' ';
	str->r = 'r';
	str->repeat[1] = (m_alarm_interval_count % 10) + '0';
	str->repeat[0] = (m_alarm_interval_count / 10) + '0';
	str->space2 = ' ';
	str->i = 'i';
	str->interval= m_alarm_interval_period;
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
