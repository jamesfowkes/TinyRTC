#ifndef _MESSAGING_H_
#define _MESSAGING_H_

enum message_id
{
    MSG_SET_RTC,
    MSG_GET_RTC,
    MSG_SET_TIMED_ACTION,
    MSG_CLEAR_TIMED_ACTION,
    MSG_SET_IO_TYPE,
    MSG_READ_INPUT,
    MSG_RESET
    MSG_INVALID
};
typedef enum message_id MESSAGE_ID;

void handleMessage(char * message);

#endif
