#ifndef _MESSAGING_H_
#define _MESSAGING_H_

/*
 * Defines and Typedefs
 */

#define MAX_MESSAGE_LENGTH (32)

// Note: these message IDs do not start at 0!
// This will affect how any loops or arrays using this enumeration are iterated/indexed!

enum message_id
{
    MSG_SET_RTC = 'A',
    MSG_GET_RTC,
    MSG_SET_ALARM,
    MSG_CLEAR_ALARM,
    MSG_SET_TRIGGER,
    MSG_CLEAR_TRIGGER,
    MSG_SET_IO_TYPE,
    MSG_READ_INPUT,
    MSG_RESET,
    _MSG_MAX_ID,
    MSG_REPLY = '>'
};
typedef enum message_id MESSAGE_ID;

#define MSG_ID_IDX(id) (id - '0')
#define MSG_MAX_ID MSG_ID_IDX(_MSG_MAX_ID)

typedef bool (*MSG_SET_RTC_FN)(TM* tm);
typedef bool (*MSG_SET_ALARM_FN)(int alarm_id, Alarm * pAlarm);
typedef bool (*MSG_CLEAR_ALARM_FN)(int alarm_id);
typedef bool (*MSG_SET_TRIGGER_FN)(int io_index, char * pTriggerExpression);
typedef bool (*MSG_CLEAR_TRIGGER_FN)(int io_index);
typedef bool (*MSG_SET_IO_TYPE_FN)(int io_index, IO_TYPE io_type);
typedef bool (*MSG_READ_INPUT_FN)(IO_STATE io_state);
typedef bool (*MSG_RESET_FN)(void);
typedef bool (*MSG_REPLY_FN)(char * buffer);

struct msg_handler_functions
{
	MSG_SET_RTC_FN set_rtc_fn;
	MSG_SET_ALARM_FN set_alarm_fn;
	MSG_CLEAR_ALARM_FN clr_alarm_fn;
	MSG_SET_TRIGGER_FN set_trigger_fn;
	MSG_CLEAR_TRIGGER_FN clear_trigger_fn;
	MSG_SET_IO_TYPE_FN set_io_type_fn;
	MSG_RESET_FN reset_fn;
	MSG_REPLY_FN reply_fn;
};
typedef struct msg_handler_functions MSG_HANDLER_FUNCTIONS;

class MessageHandler
{
	public:
		MessageHandler(MSG_HANDLER_FUNCTIONS * callbacks);
		bool handle_message(char * message);

	private:

		void new_reply(MESSAGE_ID id);

		bool set_rtc_from_message(char * message);
		bool get_rtc();
		bool set_alarm_from_message(char * message);
		bool clear_alarm_from_message(char * message);
		bool set_trigger_from_message(char * message);
		bool clear_trigger_from_message(char * message);
		bool set_io_type_from_message(char * message);
		bool read_input_from_message(char * message);
		bool reset_from_message();

		MSG_HANDLER_FUNCTIONS * m_callbacks;
};

#endif
