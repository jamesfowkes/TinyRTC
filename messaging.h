#ifndef _MESSAGING_H_
#define _MESSAGING_H_

/*
 * Defines and Typedefs
 */

enum message_id
{
    MSG_SET_RTC,
    MSG_GET_RTC,
    MSG_SET_TIMED_ACTION,
    MSG_CLEAR_TIMED_ACTION,
    MSG_SET_IO_TYPE,
    MSG_READ_INPUT,
    MSG_RESET,
    MSG_INVALID
};
typedef enum message_id MESSAGE_ID;

typedef bool (*MSG_SET_RTC_FN)(uint8_t yy, uint8_t mmm, uint8_t dd, uint8_t hh, uint8_t mm, uint8_t ss);
typedef void (*MSG_GET_RTC_FN)(void);
typedef void (*MSG_SET_TIMED_ACTION_FN)(void);
typedef void (*MSG_CLR_TIMED_ACTION_FN)(void);
typedef void (*MSG_SET_IO_TYPE_FN)(void);
typedef void (*MSG_READ_INPUT_FN)(void);
typedef void (*MSG_RESET_FN)(void);
typedef void (*MSG_INVALID_FN)(void);

struct msg_handler_functions
{
	MSG_SET_RTC_FN setRTCfn;
	MSG_GET_RTC_FN getRTCfn;
	MSG_SET_TIMED_ACTION_FN setTimedActionfn;
	MSG_CLR_TIMED_ACTION_FN getTimedActionfn;
	MSG_SET_IO_TYPE_FN setIOTypefn;
	MSG_READ_INPUT_FN readInputfn;
	MSG_RESET_FN resetfn;
	MSG_INVALID_FN invalidfn;
};
typedef struct msg_handler_functions MSG_HANDLER_FUNCTIONS;

class MessageHandler
{
	public:
		MessageHandler(char * message, MSG_HANDLER_FUNCTIONS * callbacks);
};

#endif
