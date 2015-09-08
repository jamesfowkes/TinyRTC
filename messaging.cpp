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

#include "app.rtc.h"
#include "alarm.h"
#include "messaging.h"

/*
 * Defines and typedefs
 */

struct dt_format_string
{
    char day[3];
    char space1;
    char year[2];
    char hyphen1;
    char month[2];
    char hyphen2;
    char date[2];
    char space2;
    char hour[2];
    char colon1;
    char minute[2];
    char colon2;
    char second[2];
};
typedef struct dt_format_string DT_FORMAT_STRING;

struct set_alarm_format_string
{
    char action_id[2];
    char space1;
    char repeat[2];
    char interval;
    char space2;
    char datetime_start;
};
typedef struct set_alarm_format_string SET_ALARM_FORMAT_STRING;

/*
 * Private Variables
 */

static char s_reply[MAX_MESSAGE_LENGTH];

static int textual_month_range[] = {1, 12};
static int hours_range[] = {0, 23};
static int ms_range[] = {0, 59};
static int dow_range[] = {0, 6};

/*
 * Private Functions
 */

static int * get_action_id_range()
{
    static int range[] = {0, 16};
    return range;
}

static int * get_max_repeat()
{
    static int range[] = {0, 50};
    return range;
}

static bool message_is_correct_rtc_format(char * message)
{
    // Date time format is YY-MM-DD hh:mm:ss
    // This function only validates FORMAT, not content
    DT_FORMAT_STRING * messageAsTime = (DT_FORMAT_STRING * )message;

    bool valid = true;
    
    valid &= strlen(message) == sizeof(DT_FORMAT_STRING);

    valid &= messageAsTime->hyphen1 == '-';
    valid &= messageAsTime->hyphen2 == '-';
    valid &= messageAsTime->space1 == ' ';
    valid &= messageAsTime->space2 == ' ';
    valid &= messageAsTime->colon1 == ':';
    valid &= messageAsTime->colon2 == ':';
    return valid;
}

static bool three_char_cmp(char const * const p1, char const * const p2)
{
    // Returns TRUE is p1 and p2 are the same to the 3rd char
    bool same = true;
    same &= p1[0] == p2[0];
    same &= p1[1] == p2[1];
    same &= p1[2] == p2[2];
    return same;
}

static bool parse_chars_to_day(int * pResult, char * threeCharDay)
{
    // Sets pResult to 0 for SUN, 1 for MON etc. Returns false if not a day string.
    if (!pResult) { return false; }

    if (three_char_cmp(threeCharDay, "SUN")) { *pResult = 0; return true; }
    if (three_char_cmp(threeCharDay, "MON")) { *pResult = 1; return true; }
    if (three_char_cmp(threeCharDay, "TUE")) { *pResult = 2; return true; }
    if (three_char_cmp(threeCharDay, "WED")) { *pResult = 3; return true; }
    if (three_char_cmp(threeCharDay, "THU")) { *pResult = 4; return true; }
    if (three_char_cmp(threeCharDay, "FRI")) { *pResult = 5; return true; }
    if (three_char_cmp(threeCharDay, "SAT")) { *pResult = 6; return true; }

    return false;
}

static bool parse_two_chars_to_int(int * pResult, char * twoCharNumber, int * range = NULL)
{
    // Convert ASCII chars to actual value

    if (!pResult || !twoCharNumber) { return false; }

    twoCharNumber[0] -= '0';
    twoCharNumber[1] -= '0';

    uint16_t result = (twoCharNumber[0] * 10) + twoCharNumber[1];

    bool valid = true;
    
    if (range)
    {
        valid &= result >= range[0];
        valid &= result <= range[1];
    }

    if (valid)
    {
        *pResult = result;
    }

    return valid;
}

static void weekday_to_weekday_string(int wday, char * str)
{
    // Set str to SUN for 0, MON for 1 etc...
    switch(wday)
    {
    case 0:
        str[0] = 'S'; str[1] = 'U'; str[2] = 'N';
        break;
    case 1:
        str[0] = 'M'; str[1] = 'O'; str[2] = 'N';
        break;
    case 2:
        str[0] = 'T'; str[1] = 'U'; str[2] = 'E';
        break;
    case 3:
        str[0] = 'W'; str[1] = 'E'; str[2] = 'D';
        break;
    case 4:
        str[0] = 'T'; str[1] = 'H'; str[2] = 'U';
        break;
    case 5:
        str[0] = 'F'; str[1] = 'R'; str[2] = 'I';
        break;
    case 6:
        str[0] = 'S'; str[1] = 'A'; str[2] = 'T';
        break;
    }
}

static bool time_to_datetime_string(TM* pTime, DT_FORMAT_STRING * dtString)
{
    if (!pTime) { return false; }
    if (!dtString) { return false; }

    dtString->hyphen1 = '-';
    dtString->hyphen2 = '-';
    dtString->space1 = ' ';
    dtString->space2 = ' ';
    dtString->colon1 = ':';
    dtString->colon2 = ':';

    weekday_to_weekday_string(pTime->tm_wday, dtString->day);

    dtString->year[0] = (pTime->tm_year / 10) + '0';
    dtString->year[1] = (pTime->tm_year % 10) + '0';

    dtString->month[0] = (pTime->tm_mon / 10) + '0';
    dtString->month[1] = (pTime->tm_mon % 10) + '0';

    dtString->date[0] = (pTime->tm_mday / 10) + '0';
    dtString->date[1] = (pTime->tm_mday % 10) + '0';

    dtString->hour[0] = (pTime->tm_hour / 10) + '0';
    dtString->hour[1] = (pTime->tm_hour % 10) + '0';

    dtString->minute[0] = (pTime->tm_min / 10) + '0';
    dtString->minute[1] = (pTime->tm_min % 10) + '0';

    dtString->second[0] = (pTime->tm_sec / 10) + '0';
    dtString->second[1] = (pTime->tm_sec % 10) + '0';

    return true;
}

static bool is_valid_interval(char interval)
{
    bool valid = false;
    valid |= (interval == (char)INTERVAL_HOUR);
    valid |= (interval == (char)INTERVAL_DAY);
    valid |= (interval == (char)INTERVAL_WEEK);
    valid |= (interval == (char)INTERVAL_MONTH);
    valid |= (interval == (char)INTERVAL_YEAR);
    return valid;
}

/* 
 * parse_datetime_for_interval
 *
 * For given interval, (H, D, W, M, Y), attempts to parse
 * the string given by datetime_str and validate it for
 * sanity. The basic string is mm-dd hh:mm:ss
 * but all values are optional and default to 1st January 00:00 unless string specifies them.
 */
static bool parse_datetime_for_interval(char interval, char * datetime_str, TM * datetime)
{
    bool success = true;
    int length = strlen(datetime_str);

    // Default time to 00-01-01 00:00:00
    datetime->tm_year = 0;
    datetime->tm_mon = 0; // Month from 0 to 11
    datetime->tm_mday = 1;
    datetime->tm_hour = 0;
    datetime->tm_min = 0;
    datetime->tm_sec = 0;

    /* Parse by cascading from year to hour */
    if (interval == INTERVAL_YEAR)
    {
        if (length >= 2)
        {
            if (!parse_two_chars_to_int(&datetime->tm_mon, datetime_str, textual_month_range)) { return false; }
            datetime->tm_mon--; // Shift 1-12 month indexing to 0-11

            // Next, try to parse as a monthly interval
            interval = INTERVAL_MONTH;
            datetime_str += 2; // Skip over month
            length -= 2;
            if (length && (*datetime_str != '-')) { return false; } // Check for hyphen if string continues
            datetime_str += 1; // Skip over hyphen
            length -= 1;
        }
    }

    if (interval == INTERVAL_MONTH)
    {
        if (length >= 2)
        {
            if (!parse_two_chars_to_int(&datetime->tm_mday, datetime_str)) { return false; }
            // Next, try to parse as a daily interval
            interval = INTERVAL_DAY;
            datetime_str += 2; // Skip over day-of-month
            length -= 2;
            if (length && (*datetime_str != ' ')) { return false; } // Check for space if string continues
            datetime_str += 1; // Skip over space
            length -= 1;
        }
    }

    if (interval == INTERVAL_DAY)
    {
        if (length >= 2)
        {
            if (!parse_two_chars_to_int(&datetime->tm_hour, datetime_str, hours_range)) { return false; }
            // Next, try to parse as a hourly interval
            interval = INTERVAL_HOUR;
            datetime_str += 2; // Skip over hour
            length -= 2;
            if (length && (*datetime_str != ':')) { return false; } // Check for colon if string continues
            datetime_str += 1; // Skip over colon
            length -= 1;
        }
    }

    if (interval == INTERVAL_HOUR)
    {
        if (length >= 2)
        {
            if (!parse_two_chars_to_int(&datetime->tm_min, datetime_str, ms_range)) { return false; }
        }
    }

    return success;
}

MessageHandler::MessageHandler(MSG_HANDLER_FUNCTIONS * callbacks)
{
    m_callbacks  = callbacks;
}

bool MessageHandler::handleMessage(char * message)
{
    MESSAGE_ID id = (MESSAGE_ID)message[0];
    
    bool result = false;
    
    if (!m_callbacks) { return false; }

    switch(id)
    {
    case MSG_SET_RTC:
        result = setRTCFromMessage(&message[1]);
        break;
    case MSG_GET_RTC:
        result = getRTC();
        break;
    case MSG_SET_ALARM:
        result = setAlarmFromMessage(&message[1]);
        break;
    case MSG_CLEAR_ALARM:
        result = false;//clearAlarmFromMessage(&message[1]);
        break;
    case MSG_SET_IO_TYPE:
        result = false;//setIOTypeFromMessage(&message[1]);
        break;
    case MSG_READ_INPUT:
        result = false;//printInputStateFromMessage(&message[1]));
    case MSG_RESET:
        result = false;//resetAllActions(&message[1]);
        break;
    default:
        break;
    }
    
    return result;
}

bool MessageHandler::setRTCFromMessage(char * message)
{
    bool result = false;

    if (!m_callbacks->setRTCfn) { return false; }

    TM new_time;

    if (!message_is_correct_rtc_format(message)) { return false; }

    // Convert each part of the message to integer
    DT_FORMAT_STRING * messageAsTime = (DT_FORMAT_STRING*)message;

    if (!parse_chars_to_day(&new_time.tm_wday, messageAsTime->day)) { return false; }
    if (!parse_two_chars_to_int(&new_time.tm_year, messageAsTime->year)) { return false; }
    if (!parse_two_chars_to_int(&new_time.tm_mon, messageAsTime->month, textual_month_range)) { return false; }
    if (!parse_two_chars_to_int(&new_time.tm_mday, messageAsTime->date)) { return false; }
    if (!parse_two_chars_to_int(&new_time.tm_hour, messageAsTime->hour, hours_range)) { return false; }
    if (!parse_two_chars_to_int(&new_time.tm_min, messageAsTime->minute, ms_range)) { return false; }
    if (!parse_two_chars_to_int(&new_time.tm_sec, messageAsTime->second, ms_range)) { return false; }
    
    new_time.tm_mon--; // Shift 1-12 month indexing to 0-11

    if (!days_in_month_valid(new_time.tm_mday, new_time.tm_mon, new_time.tm_year)) { return 0; }

    // Got this far, everything is valid
    result = m_callbacks->setRTCfn(&new_time);

    return result;
}

bool MessageHandler::getRTC()
{
    bool result = false;

    if (!m_callbacks->replyfn) { return false; }

    TM tm;
    APP_GetRTCDatetime(&tm);

    tm.tm_mon++; // Convert 0-11 to 1-12 for conversion to string date
    
    memset(s_reply, '\0', MAX_MESSAGE_LENGTH);

    // Set message header
    s_reply[0] = MSG_REPLY;
    s_reply[1] = MSG_GET_RTC;

    time_to_datetime_string(&tm, (DT_FORMAT_STRING*)&s_reply[2]);

    result = m_callbacks->replyfn(s_reply);

    return result;
}

bool MessageHandler::setAlarmFromMessage(char * message)
{
    bool result = false;
    int action_id;
    int repeat = 0;
    ALARM new_alarm;
    TM alarm_time;

    if (!m_callbacks->setAlarmfn) { return false; }
    
    SET_ALARM_FORMAT_STRING * message_as_set_alarm_string = (SET_ALARM_FORMAT_STRING*)message;

    if (!parse_two_chars_to_int(&action_id, message_as_set_alarm_string->action_id, get_action_id_range())) { return false; }
    if (!parse_two_chars_to_int(&repeat, message_as_set_alarm_string->repeat, get_max_repeat())) { return false; }

    if (!is_valid_interval(message_as_set_alarm_string->interval)) { return false; }

    if (!parse_datetime_for_interval(
        message_as_set_alarm_string->interval,
        &message_as_set_alarm_string->datetime_start,
        &alarm_time))
    {
        return false; 
    }

    if (!days_in_month_valid(alarm_time.tm_mday, alarm_time.tm_mon, alarm_time.tm_year)) { return 0; }

    result = ALARM_make(&new_alarm, (INTERVAL)message_as_set_alarm_string->interval, &alarm_time, repeat);

    result &= m_callbacks->setAlarmfn(action_id, &new_alarm);

    return result;
}
