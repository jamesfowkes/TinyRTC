#ifndef _MESSAGING_H_
#define _MESSAGING_H_

/*
 * Defines and Typedefs
 */

#define MAX_MESSAGE_LENGTH (32)

// Note: these message IDs start at character '0', not 0!
// This will affect how any loops or arrays using this enumeration are iterated/indexed!

enum message_id
{
    MSG_SET_RTC = '0',
    MSG_GET_RTC,
    MSG_SET_TIMED_ACTION,
    MSG_CLEAR_TIMED_ACTION,
    MSG_SET_IO_TYPE,
    MSG_READ_INPUT,
    MSG_RESET,
    MSG_INVALID,
    MSG_REPLY,
    _MSG_MAX_ID
};
typedef enum message_id MESSAGE_ID;

#define MSG_ID_IDX(id) (id - '0')
#define MSG_MAX_ID MSG_ID_IDX(_MSG_MAX_ID)

typedef bool (*MSG_SET_RTC_FN)(TM* tm);
typedef void (*MSG_SET_TIMED_ACTION_FN)(void);
typedef void (*MSG_CLR_TIMED_ACTION_FN)(void);
typedef void (*MSG_SET_IO_TYPE_FN)(void);
typedef void (*MSG_READ_INPUT_FN)(void);
typedef void (*MSG_RESET_FN)(void);
typedef void (*MSG_INVALID_FN)(void);
typedef void (*MSG_REPLY_FN)(char * buffer);

struct msg_handler_functions
{
	MSG_SET_RTC_FN setRTCfn;
	MSG_SET_TIMED_ACTION_FN setTimedActionfn;
	MSG_CLR_TIMED_ACTION_FN getTimedActionfn;
	MSG_SET_IO_TYPE_FN setIOTypefn;
	MSG_READ_INPUT_FN readInputfn;
	MSG_RESET_FN resetfn;
	MSG_INVALID_FN invalidfn;
	MSG_REPLY_FN replyfn;
};
typedef struct msg_handler_functions MSG_HANDLER_FUNCTIONS;

class MessageHandler
{
	public:
		MessageHandler(MSG_HANDLER_FUNCTIONS * callbacks);
		bool handleMessage(char * message);

	private:
		bool setRTCFromMessage(char * message);
		bool getRTC();

		MSG_HANDLER_FUNCTIONS * m_callbacks;
};

#endif
