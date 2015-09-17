#ifndef _ALARM_H_
#define _ALARM_H_

#ifdef TEST
#include <string>
#endif

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

/*
 * Public Function Prototypes
 */

struct alarm_string
{
	DT_FORMAT_STRING datetime;
	char space1;
	char r;
	char repeat[2];
	char space2;
	char i;
	char interval;
	char space3;
	char d;
	char duration[5];
	char null;
};
typedef struct alarm_string ALARM_STRING;

class Alarm
{
public:
	Alarm();
	Alarm(INTERVAL interval, TM * time, int repeat, int duration);

	friend bool operator==(const Alarm& lhs, const Alarm& rhs);

	void reset();
	bool valid() { return m_valid; }
	void print(char * buffer);

	#ifdef TEST
	int get_repeat() { return m_repeat; }
	#endif

	bool to_string(ALARM_STRING * str) const;

	bool set_time(TM * time);
	bool is_triggered() { return m_triggered; }
private:
	TM m_datetime;
	int m_repeat;	// Repeat number - e.g if this is 2 for a weekly interval, the alarm will go off fortnightly.
	INTERVAL m_repeat_interval; // Type of repeat
	int m_duration; // How long alarm lasts
	bool m_triggered;
	bool m_valid;
};

#ifdef TEST
namespace CppUnit
{
	template<>
	struct assertion_traits<Alarm>   // specialization for the Alarm type
	{
		static bool equal(const Alarm& lhs, const Alarm& rhs)
		{
			return lhs == rhs;
		}

		static std::string toString(const Alarm& alarm)
		{
			ALARM_STRING buffer;
			(void)alarm.to_string(&buffer);
			return "'" + std::string((char*)&buffer) + "'";
		}
	};
};
#endif

void set_default_alarm_time(TM* tm);

#endif
