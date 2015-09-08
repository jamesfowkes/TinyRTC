#ifndef _ALARM_H_
#define _ALARM_H_

struct alarm_struct
{
	TM datetime;
	uint16_t repeat;
};
typedef struct alarm_struct ALARM;

enum interval
{
	INTERVAL_HOUR = 'H',
	INTERVAL_DAY = 'D',
	INTERVAL_WEEK = 'W',
	INTERVAL_MONTH = 'M',
	INTERVAL_YEAR = 'Y'
};
typedef enum interval INTERVAL;

bool ALARM_make(ALARM * alarm, INTERVAL interval, TM * time, uint16_t repeat);

#endif
