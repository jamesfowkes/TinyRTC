#ifndef _ALARM_H_
#define _ALARM_H_

/*
 * Defines and typedefs
 */

enum interval
{
	INTERVAL_HOUR = 'H',
	INTERVAL_DAY = 'D',
	INTERVAL_WEEK = 'W',
	INTERVAL_MONTH = 'M',
	INTERVAL_YEAR = 'Y'
};
typedef enum interval INTERVAL;

struct alarm_struct
{
	TM datetime;
	int repeat;	// Repeat number - e.g if this is 2 for a weekly interval, the alarm will go off fortnightly.
	INTERVAL repeat_interval; // Type of repeat
	int duration; // How long alarm lasts
	bool triggered;
};
typedef struct alarm_struct ALARM;

/*
 * Public Function Prototypes
 */
bool alarm_make(ALARM * alarm, INTERVAL interval, TM * time, int repeat, int duration);

#endif
