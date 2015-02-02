#include <stdbool.h>

#include "util_time.h"

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
    valid &= message[13] == ':';
    
    return valid
}

static bool parseMessageToDatePart(char * pResult, char tensASCII, char unitsASCII);
{
    // Convert ASCII chars to actual value
    tensASCII -= '0';
    unitsASCII -= '0';
    
    *pResult = (tensASCII * 10) + unitsASCII;
}

static bool setRTCFromMessage(char * message)
{

    uint8_t dateParts[6]; // Holds yy, mm, dd, hh, mm, ss
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
    int daysInMonth = days_in_month(dateParts[1], isLeapYear(fourDigitYear));
    if (dateParts[1] > 12) { return; }
    if (dateParts[2] > daysInMonth) { return; }
    if (dateParts[3] > 23) { return; }
    if (dateParts[4] > 59) { return; }
    if (dateParts[5] > 59) { return; }
    
    // Got this far, everything is valid
    setRTC(dateParts[0], dateParts[1], dateParts[2], dateParts[3], dateParts[4], dateParts[5]);
}

static bool setTimedActionFromMessage(char * message)
{

}

bool handleMessage(char * message);
{
    MESSAGE_ID id = (MESSAGE_ID)message[0];
    
    bool result = false;
    
    switch(id)
    {
    case MSG_SET_RTC:
        result = setRTCFromMessage(&message[1]);
        break;
    case MSG_GET_RTC:
        result = printRTC();
        break;
    case MSG_SET_TIMED_ACTION:
        result = setTimedActionFromMessage(&message[1]);
        break;
    case MSG_CLEAR_TIMED_ACTION:
        result = clearTimedActionFromMessage(&message[1]);
        break;
    case MSG_SET_IO_TYPE:
        result = setIOTypeFromMessage(&message[1]);
        break;
    case MSG_READ_INPUT:
        result = printInputStateFromMessage(&message[1]));
    case MSG_RESET:
        result = resetAllActions(&message[1]);
        break;
    default:
        break;
    }
    
    return result;
}
