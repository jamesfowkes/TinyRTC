/*
 * C Library Includes
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#ifdef TEST
#include <cppunit/TestAssert.h>
#endif

/*
 * Code Library Includes
 */

#include "Utility/util_time.h"
#include "Utility/util_simple_compare.h"
#include "Utility/util_simple_parse.h"

/*
 * Application Includes
 */

#include "io.h"
#include "app.rtc.h"
#include "alarm.h"
#include "messaging.h"

/*
 * Defines and typedefs
 */

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
//static int dow_range[] = {0, 6};

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
    static int range[] = {1, 50};
    return range;
}

static int * get_input_index_range()
{
    static int range[] = {0, 3};
    return range;
}

static bool message_is_correct_rtc_format(char * message)
{
    // Date time format is YY-MM-DD hh:mm:ss
    // This function only validates FORMAT, not content
    DT_FORMAT_STRING * message_as_time = (DT_FORMAT_STRING * )message;

    bool valid = true;
    
    valid &= strlen(message) == sizeof(DT_FORMAT_STRING);

    valid &= message_as_time->hyphen1 == '-';
    valid &= message_as_time->hyphen2 == '-';
    valid &= message_as_time->space1 == ' ';
    valid &= message_as_time->space2 == ' ';
    valid &= message_as_time->colon1 == ':';
    valid &= message_as_time->colon2 == ':';
    return valid;
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
static bool parse_datetime_for_interval(char interval, char const * datetime_str, TM * datetime)
{
    bool success = true;
    int length = strlen(datetime_str);

    /* Parse by cascading from year to hour */
    if (interval == INTERVAL_YEAR)
    {
        if (length >= 2)
        {
            if (!parse_chars_to_int(&datetime->tm_mon, datetime_str, 2, textual_month_range)) { return false; }
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
            if (!parse_chars_to_int(&datetime->tm_mday, datetime_str, 2)) { return false; }
            // Next, try to parse as a daily interval
            interval = INTERVAL_DAY;
            datetime_str += 2; // Skip over day-of-month
            length -= 2;
            if (length && (*datetime_str != ' ')) { return false; } // Check for space if string continues
            datetime_str += 1; // Skip over space
            length -= 1;
        }
    }

    if (interval == INTERVAL_WEEK)
    {
        // Special case where formatting is DDD hh:mm where DDD is three-letter day.
        if (length >= 3)
        {
            if (!chars_to_weekday(&datetime->tm_wday, datetime_str)) { return false; }
            // Next, try to parse as a daily interval (which will just be hh:mm)
            interval = INTERVAL_DAY;
            datetime_str += 3; // Skip over month
            length -= 3;
            if (length && (*datetime_str != ' ')) { return false; } // Check for space if string continues
            datetime_str += 1; // Skip over space
            length -= 1;
        }
    }

    if (interval == INTERVAL_DAY)
    {
        if (length >= 2)
        {
            if (!parse_chars_to_int(&datetime->tm_hour, datetime_str, 2, hours_range)) { return false; }
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
            if (!parse_chars_to_int(&datetime->tm_min, datetime_str, 2, ms_range)) { return false; }
        }
    }

    return success;
}

/* 
 * try_get_duration_from_message
 *
 * A message may be suffixed with D where x is a number representing the number of minutes
 * the alarm should be active for.
 * The function works backwards from the end of the message to find the number (if there is one)
 * 0 is returned otherwise.
 */

static int try_get_duration_from_message(char * message)
{
    int result = 0;
    int multiplier = 1;

    int c = strlen(message) - 1; // Start at end of message
    while (isdigit(message[c]) && (c >= 0))
    {
        result += (message[c] - '0') * multiplier;
        multiplier *= 10;
        c--;
    }

    if (c < 0) { return 0; } // Ran out of chars to process

    if (message[c] != 'D') { return 0;} // Digits not prefixed with 'D'

    return result;
}

MessageHandler::MessageHandler(MSG_HANDLER_FUNCTIONS * callbacks)
{
    m_callbacks  = callbacks;
}

void MessageHandler::new_reply(MESSAGE_ID id)
{
    memset(s_reply, '\0', MAX_MESSAGE_LENGTH);
    s_reply[0] = MSG_REPLY;
    s_reply[1] = (char)id;
}

bool MessageHandler::handle_message(char * message)
{
    if (!message) { return false; }

    MESSAGE_ID id = (MESSAGE_ID)message[0];
    
    bool result = false;
    
    bool send_standard_reply = true;

    if (!m_callbacks) { return false; }

    switch(id)
    {
    case MSG_SET_RTC:
        result = set_rtc_from_message(&message[1]);
        break;
    case MSG_GET_RTC:
        result = get_rtc();
        send_standard_reply = false;
        break;
    case MSG_SET_ALARM:
        result = set_alarm_from_message(&message[1]);
        break;
    case MSG_CLEAR_ALARM:
        result = clear_alarm_from_message(&message[1]);
        break;
    case MSG_SET_TRIGGER:
        result = set_trigger_from_message(&message[1]);
        break;
    case MSG_CLEAR_TRIGGER:
        result = clear_trigger_from_message(&message[1]);
        break;
    case MSG_SET_IO_TYPE:
        result = set_io_type_from_message(&message[1]);
        break;
    case MSG_READ_INPUT:
        result = read_input_from_message(&message[1]);
        send_standard_reply = false;
        break;
    case MSG_RESET:
        result = reset_from_message();
        send_standard_reply = false;
        break;
    default:
        break;
    }

    if (send_standard_reply)
    {
        s_reply[0] = MSG_REPLY;
        s_reply[1] = id;
        strncpy(&s_reply[2], result ? " OK" : " FAIL",MAX_MESSAGE_LENGTH-2);
        m_callbacks->reply_fn(s_reply);
    }
    
    return result;
}

bool MessageHandler::set_rtc_from_message(char * message)
{
    bool result = false;

    if (!m_callbacks->set_rtc_fn) { return false; }

    TM new_time;

    if (!message_is_correct_rtc_format(message)) { return false; }

    // Convert each part of the message to integer
    DT_FORMAT_STRING * message_as_time = (DT_FORMAT_STRING*)message;

    if (!chars_to_weekday(&new_time.tm_wday, message_as_time->day)) { return false; }
    if (!parse_chars_to_int(&new_time.tm_year, message_as_time->year, 2)) { return false; }
    if (!parse_chars_to_int(&new_time.tm_mon, message_as_time->month, 2, textual_month_range)) { return false; }
    if (!parse_chars_to_int(&new_time.tm_mday, message_as_time->date, 2)) { return false; }
    if (!parse_chars_to_int(&new_time.tm_hour, message_as_time->hour, 2, hours_range)) { return false; }
    if (!parse_chars_to_int(&new_time.tm_min, message_as_time->minute, 2, ms_range)) { return false; }
    if (!parse_chars_to_int(&new_time.tm_sec, message_as_time->second, 2, ms_range)) { return false; }
    
    new_time.tm_mon--; // Shift 1-12 month indexing to 0-11

    if (!days_in_month_valid(new_time.tm_mday, new_time.tm_mon, new_time.tm_year)) { return 0; }

    // Got this far, everything is valid
    result = m_callbacks->set_rtc_fn(&new_time);

    return result;
}

bool MessageHandler::get_rtc()
{
    bool result = false;

    if (!m_callbacks->reply_fn) { return false; }

    TM tm;
    app_get_rtc_datetime(&tm);
   
    new_reply(MSG_GET_RTC);
    time_to_datetime_string(&tm, (DT_FORMAT_STRING*)&s_reply[2]);

    result = m_callbacks->reply_fn(s_reply);

    return result;
}

bool MessageHandler::set_alarm_from_message(char * message)
{
    bool result = false;
    int action_id;
    int repeat = 0;
    
    TM alarm_time;
    set_default_alarm_time(&alarm_time);

    if (!message) { return false; }
    if (!m_callbacks->set_alarm_fn) { return false; }
    if (strlen(message) == 0) { return false;}

    SET_ALARM_FORMAT_STRING * message_as_set_alarm_string = (SET_ALARM_FORMAT_STRING*)message;

    if (!parse_chars_to_int(&action_id, message_as_set_alarm_string->action_id, 2, get_action_id_range())) { return false; }
    if (!parse_chars_to_int(&repeat, message_as_set_alarm_string->repeat, 2, get_max_repeat())) { return false; }

    if (!is_valid_interval(message_as_set_alarm_string->interval)) { return false; }

    if (!parse_datetime_for_interval(
        message_as_set_alarm_string->interval,
        &message_as_set_alarm_string->datetime_start,
        &alarm_time))
    {
        return false; 
    }

    int duration = try_get_duration_from_message(message);

    if (!days_in_month_valid(alarm_time.tm_mday, alarm_time.tm_mon, alarm_time.tm_year)) { return 0; }

    Alarm new_alarm = Alarm((INTERVAL)message_as_set_alarm_string->interval, &alarm_time, repeat, duration);

    result = new_alarm.valid();
    result &= m_callbacks->set_alarm_fn(action_id, &new_alarm);

    return result;
}

bool MessageHandler::clear_alarm_from_message(char * message)
{
    bool result = false;
    int action_id;

    if (!m_callbacks->clr_alarm_fn) { return false; }

    if (!parse_chars_to_int(&action_id, message, 2, get_action_id_range())) { return false; }

    result = m_callbacks->clr_alarm_fn(action_id);

    return result;
}

bool MessageHandler::set_trigger_from_message(char * message)
{
    bool result = false;
    int io_index;

    if (!m_callbacks->set_trigger_fn) { return false; }

    if (message[1] != ' ') { return false; }
    if (!parse_chars_to_int(&io_index, &message[0], 1, get_input_index_range())) { return false; }
    
    result = m_callbacks->set_trigger_fn(io_index, &message[2]);

    return result;
}

bool MessageHandler::clear_trigger_from_message(char * message)
{
    bool result = false;
    int io_index;

    if (!m_callbacks->clear_trigger_fn) { return false; }

    if (!parse_chars_to_int(&io_index, &message[0], 1, get_input_index_range())) { return false; }

    result = m_callbacks->clear_trigger_fn(io_index);

    return result;
}

bool MessageHandler::set_io_type_from_message(char * message)
{
    bool result = false;
    int io_index;
    IO_TYPE io_type;
    
    if (!m_callbacks->set_io_type_fn) { return false; }

    if (message[1] != ' ') { return false; }

    if (!parse_chars_to_int(&io_index, &message[0], 1, get_input_index_range())) { return false; }

    if (!parse_chars_to_io_type(&io_type, &message[2])) { return false; }

    result = m_callbacks->set_io_type_fn(io_index, io_type);

    return result;  
}

bool MessageHandler::read_input_from_message(char * message)
{
    bool result = false;
    int io_index = -1;

    if (!m_callbacks->reply_fn) { return false; }

    if (!parse_chars_to_int(&io_index, &message[0], 1, get_input_index_range())) { return false; }

    new_reply(MSG_READ_INPUT);

    IO_STATE state = app_get_io_state(io_index);

    switch(state)
    {
    case OFF:
        s_reply[2] = '0';
        break;
    case ON:
        s_reply[2] = '1';
        break;
    case UNKNOWN:
    default:
        s_reply[2] = '?';
        break;
    }

    s_reply[3] = '\0';

    result = m_callbacks->reply_fn(s_reply);    

    return result;
}

bool MessageHandler::reset_from_message()
{
    bool result = false;

    if (!m_callbacks->reset_fn) { return false; }

    new_reply(MSG_RESET);
    strncpy(&s_reply[2], "RESET", MAX_MESSAGE_LENGTH-2);
    
    (void)m_callbacks->reply_fn(s_reply);
    result = m_callbacks->reset_fn();

    return result;    
}
