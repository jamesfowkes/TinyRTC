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

#include "messaging.h"

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

static bool parseMessageToDatePart(uint8_t * pResult, char tensASCII, char unitsASCII)
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
        result = false;//printRTC();
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

    uint8_t dateParts[6]; // Holds yy, mmm, dd, hh, mm, ss
    if (!messageIsCorrectRTCFormat(message)) { return false; }

    // Convert each part of the message to integer
    if (!parseMessageToDatePart(&dateParts[0], message[0], message[1])) { return false; }
    if (!parseMessageToDatePart(&dateParts[1], message[3], message[4])) { return false; }
    if (!parseMessageToDatePart(&dateParts[2], message[6], message[7])) { return false; }
    if (!parseMessageToDatePart(&dateParts[3], message[9], message[10])) { return false; }
    if (!parseMessageToDatePart(&dateParts[4], message[12], message[13])) { return false; }
    if (!parseMessageToDatePart(&dateParts[5], message[15], message[16])) { return false; }
    
    // Validate month, date, hour, minute and second
    GREGORIAN_YEAR fourDigitYear = 2000 + dateParts[0];
    int daysInMonth = days_in_month(dateParts[1]-1, is_leap_year(fourDigitYear));
    if ((dateParts[1] > 12)|| (dateParts[1] == 0)) { return false; } // Month between 1 and 12
    if ((dateParts[2] > daysInMonth) || (dateParts[2] == 0)) { return false; } // Date between 1 and <days in month>
    if (dateParts[3] > 23) { return false; }
    if (dateParts[4] > 59) { return false; }
    if (dateParts[5] > 59) { return false; }

    // Got this far, everything is valid
    result = m_callbacks->setRTCfn(dateParts[0], dateParts[1], dateParts[2], dateParts[3], dateParts[4], dateParts[5]);

    return result;
}