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
    MSG_SET_RTC = 'A',
    MSG_GET_RTC,
    MSG_SET_ALARM,
    MSG_CLEAR_ALARM,
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
typedef bool (*MSG_SET_ALARM_FN)(int action_id, ALARM * pAlarm);
typedef bool (*MSG_CLR_ALARM_FN)(int action_id);
typedef bool (*MSG_SET_IO_TYPE_FN)(void);
typedef bool (*MSG_READ_INPUT_FN)(void);
typedef bool (*MSG_RESET_FN)(void);
typedef bool (*MSG_INVALID_FN)(void);
typedef bool (*MSG_REPLY_FN)(char * buffer);

struct msg_handler_functions
{
	MSG_SET_RTC_FN set_rtc_fn;
	MSG_SET_ALARM_FN set_alarm_fn;
	MSG_CLR_ALARM_FN clr_alarm_fn;
	MSG_SET_IO_TYPE_FN set_io_type_fn;
	MSG_READ_INPUT_FN read_input_fn;
	MSG_RESET_FN reset_fn;
	MSG_INVALID_FN invalid_fn;
	MSG_REPLY_FN reply_fn;
};
typedef struct msg_handler_functions MSG_HANDLER_FUNCTIONS;

class MessageHandler
{
	public:
		MessageHandler(MSG_HANDLER_FUNCTIONS * callbacks);
		bool handle_message(char * message);

	private:
		bool set_rtc_from_message(char * message);
		bool get_rtc();
		bool set_alarm_from_message(char * message);
		bool clear_alarm_from_message(char * message);

		MSG_HANDLER_FUNCTIONS * m_callbacks;
};

#endif
