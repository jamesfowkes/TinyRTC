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
#include "messaging.h"

static char s_reply[MAX_MESSAGE_LENGTH];

static bool messageIsCorrectRTCFormat(char * message)
{
    // Date time format is YY-MM-DD hh:mm:ss
    // This function only validates FORMAT, not content
    bool valid = true;
    
    valid &= strlen(message) == 17;
    valid &= message[2] == '-';
    valid &= message[5] == '-';
    valid &= message[8] == ' ';
    valid &= message[11] == ':';
    valid &= message[14] == ':';
    
    return valid;
}

static bool parseMessageToDatePart(int * pResult, char tensASCII, char unitsASCII)
{
    // Convert ASCII chars to actual value
    tensASCII -= '0';
    unitsASCII -= '0';

    uint16_t result = (tensASCII * 10) + unitsASCII;

    bool valid = result <= 59;

    if (valid)
    {
        *pResult = result;
    }

    return valid;
}

/*static bool setTimedActionFromMessage(char * message)
{
    (void)message;
    return false;
}*/

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
    case MSG_SET_TIMED_ACTION:
        result = false;//setTimedActionFromMessage(&message[1]);
        break;
    case MSG_CLEAR_TIMED_ACTION:
        result = false;//clearTimedActionFromMessage(&message[1]);
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

    TM newTime;
    if (!messageIsCorrectRTCFormat(message)) { return false; }

    // Convert each part of the message to integer
    if (!parseMessageToDatePart(&newTime.tm_year, message[0], message[1])) { return false; }
    if (!parseMessageToDatePart(&newTime.tm_mon, message[3], message[4])) { return false; }
    if (!parseMessageToDatePart(&newTime.tm_mday, message[6], message[7])) { return false; }
    if (!parseMessageToDatePart(&newTime.tm_hour, message[9], message[10])) { return false; }
    if (!parseMessageToDatePart(&newTime.tm_min, message[12], message[13])) { return false; }
    if (!parseMessageToDatePart(&newTime.tm_sec, message[15], message[16])) { return false; }
    
    // Validate month, date, hour, minute and second
    GREGORIAN_YEAR fourDigitYear = 2000 + newTime.tm_year;
    int daysInMonth = days_in_month(newTime.tm_mon-1, is_leap_year(fourDigitYear));
    if ((newTime.tm_mon > 12)|| (newTime.tm_mon == 0)) { return false; } // Month between 1 and 12
    if ((newTime.tm_mday > daysInMonth) || (newTime.tm_mday == 0)) { return false; } // Date between 1 and <days in month>
    if (newTime.tm_hour > 23) { return false; }
    if (newTime.tm_min > 59) { return false; }
    if (newTime.tm_sec > 59) { return false; }

    // Got this far, everything is valid
    result = m_callbacks->setRTCfn(&newTime);

    return result;
}

bool MessageHandler::getRTC()
{
    if (!m_callbacks->replyfn) { return false; }

    TM tm;
    APP_GetRTCDatetime(&tm);

    char c = 0;
    
    //Header
    s_reply[c++] = MSG_REPLY;
    s_reply[c++] = MSG_GET_RTC;
    
    // yy
    s_reply[c++] = (tm.tm_year / 10) + '0';
    s_reply[c++] = (tm.tm_year % 10) + '0';
    s_reply[c++] = '-';

    // mmm
    s_reply[c++] = (tm.tm_mon / 10) + '0';
    s_reply[c++] = (tm.tm_mon % 10) + '0';
    s_reply[c++] = '-';

    // dd
    s_reply[c++] = (tm.tm_mday / 10) + '0';
    s_reply[c++] = (tm.tm_mday % 10) + '0';
    s_reply[c++] = ' ';

    // hh
    s_reply[c++] = (tm.tm_hour / 10) + '0';
    s_reply[c++] = (tm.tm_hour % 10) + '0';
    s_reply[c++] = ':';

    // mm
    s_reply[c++] = (tm.tm_min / 10) + '0';
    s_reply[c++] = (tm.tm_min % 10) + '0';
    s_reply[c++] = ':';

    // ss
    s_reply[c++] = (tm.tm_sec / 10) + '0';
    s_reply[c++] = (tm.tm_sec % 10) + '0';

    s_reply[c++] = '\0';

    m_callbacks->replyfn(s_reply);

    return true;
}