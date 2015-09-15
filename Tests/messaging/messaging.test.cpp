/*
 * C Library Includes
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include "Utility/util_time.h"

#include "alarm.h"
#include "io.h"
#include "messaging.h"
#include "ast_node.h"
#include "syntax_parser.h"

#include "app.io.h"

static bool set_rtc_callback(TM* tm);
static bool set_alarm_callback(int alarm_id, ALARM * pAlarm);
static bool clr_alarm_callback(int alarm_id);
static bool set_trigger_callback(int io_index, char * pTriggerExpression);
static bool clear_trigger_callback(int io_index);
static bool set_io_type_callback(int io_index, IO_TYPE io_type);
static bool reset_callback(void);
static bool reply_callback(char * message);

static void set_test_object(void* obj);

class MessagingTest : public CppUnit::TestFixture  {

   CPPUNIT_TEST_SUITE(MessagingTest);
   CPPUNIT_TEST(ValidSetRTCMessageTest);
   CPPUNIT_TEST(InvalidSetRTCMessageTest);
   CPPUNIT_TEST(GetRTCMessageTest);
   CPPUNIT_TEST(SetAlarmMessageTestWithNoMessage);
   CPPUNIT_TEST(SetAlarmMessageTestWithWeeklyInterval);
   CPPUNIT_TEST(SetAlarmMessageTestWithFullDatetime);
   CPPUNIT_TEST(SetAlarmMessageTestWithDuration);
   CPPUNIT_TEST(SetAlarmMessageTestWithPartialDatetime);
   CPPUNIT_TEST(SetAlarmMessageTestWithNoDatetime);
   CPPUNIT_TEST(SetAlarmMessageTestWithBadDatetime);
   CPPUNIT_TEST(SetAlarmMessageTestActionID);
   CPPUNIT_TEST(SetAlarmMessageTestRepeatCount);
   CPPUNIT_TEST(ClrAlarmMessageTest);
   CPPUNIT_TEST(SetTriggerMessageTest);
   CPPUNIT_TEST(ClearTriggerMessageTest);
   CPPUNIT_TEST(SetIOTypeMessageTest);
   CPPUNIT_TEST(ReadInputMessageTest);
   CPPUNIT_TEST(ResetMessageTest);
   CPPUNIT_TEST_SUITE_END();

public:

   bool Set_rtc_callback(TM* tm)
   {
      m_callback_flags[MSG_ID_IDX(MSG_SET_RTC)] = true;
      time_cpy(&m_time, tm);
      return true;
   }

   bool Set_alarm_callback(int alarm_id, ALARM * pAlarm)
   {
      m_callback_flags[MSG_ID_IDX(MSG_SET_ALARM)] = true;
      time_cpy(&(m_alarm.datetime), &(pAlarm->datetime));
      m_alarm.repeat = pAlarm->repeat;
      m_alarm.repeat_interval = pAlarm->repeat_interval;
      m_alarm.duration = pAlarm->duration;
      m_alarm_id = alarm_id;
      return true;
   }

   bool Clr_alarm_callback(int alarm_id)
   {
      m_callback_flags[MSG_ID_IDX(MSG_CLEAR_ALARM)] = true;
      m_alarm_id = alarm_id;
      return true;
   }

   bool Set_trigger_callback(int io_index, char * pTriggerExpression)
   {
      m_reply = std::string(pTriggerExpression);
      m_io_trigger = io_index;
      m_callback_flags[MSG_ID_IDX(MSG_SET_TRIGGER)] = true;
      return true;
   }

   bool Clear_trigger_callback(int io_index)
   {
      m_io_trigger = io_index;
      m_callback_flags[MSG_ID_IDX(MSG_CLEAR_TRIGGER)] = true;
      return true;
   }

   bool Set_io_type_callback(int io_index, IO_TYPE io_type)
   {
      m_callback_flags[MSG_ID_IDX(MSG_SET_IO_TYPE)] = true;
      m_io_index = io_index;
      m_io_type = io_type;
      return true;
   }

   bool Reset_callback(void)
   {
      m_callback_flags[MSG_ID_IDX(MSG_RESET)] = true;
      return true;
   }

   bool Reply_callback(char * message)
   {
      m_reply = std::string(message);
      m_callback_flags[MSG_ID_IDX(MSG_REPLY)] = true;
      return true;
   }

   void setUp(void)
   {
      memset(m_callback_flags, false, MSG_MAX_ID);
      m_message_handler = new MessageHandler(&m_callbacks);

      m_callbacks.set_rtc_fn = set_rtc_callback;
      m_callbacks.set_alarm_fn = set_alarm_callback;
      m_callbacks.clr_alarm_fn = clr_alarm_callback;
      m_callbacks.set_trigger_fn = set_trigger_callback;
      m_callbacks.clear_trigger_fn = clear_trigger_callback;
      m_callbacks.set_io_type_fn = set_io_type_callback;
      m_callbacks.reset_fn = reset_callback;
      m_callbacks.reply_fn = reply_callback;

      m_alarm.datetime.tm_year = -1;
      m_alarm.datetime.tm_mon = -1;
      m_alarm.datetime.tm_mday = -1;
      m_alarm.datetime.tm_wday = -1;
      m_alarm.datetime.tm_hour = -1;
      m_alarm.datetime.tm_min = -1;
      m_alarm.datetime.tm_sec = -1;
      m_alarm.datetime.tm_yday = -1;
      m_alarm.datetime.tm_isdst = -1;

      m_alarm.repeat = -1;
      m_alarm.repeat_interval = (INTERVAL)-1;
      m_alarm.duration = -1;
      m_alarm_id = -1;


      m_io_index = -1;
      m_io_type = (IO_TYPE)-1;

      m_io_trigger = -1;

      set_test_object(this);
   }

   void tearDown(void)
   {
      free(m_message_handler);
   }

private:
   
   bool m_callback_flags[MSG_MAX_ID];

   TM m_time;
   std::string m_reply;
   
   ALARM m_alarm;
   int m_alarm_id;

   IO_TYPE m_io_type;
   int m_io_index;

   int m_io_trigger;

   MessageHandler * m_message_handler;

   MSG_HANDLER_FUNCTIONS m_callbacks;

   int callbackSetCount()
   {
      uint8_t count = 0;

      for(int i = 0; i < MSG_MAX_ID; ++i)
      {
         if (m_callback_flags[i]) { count++; }
      }

      return count;
   }

protected:

   void ValidSetRTCMessageTest()
   {
      char message[MAX_MESSAGE_LENGTH] = {MSG_SET_RTC};
      strncpy(&message[1], "SAT 15-08-01 18:07:37", MAX_MESSAGE_LENGTH);
      CPPUNIT_ASSERT(m_message_handler->handle_message(message));
      CPPUNIT_ASSERT(m_callback_flags[MSG_ID_IDX(MSG_SET_RTC)]);

      CPPUNIT_ASSERT_EQUAL(15, m_time.tm_year);
      CPPUNIT_ASSERT_EQUAL(7, m_time.tm_mon); // Month from 0 to 11
      CPPUNIT_ASSERT_EQUAL(1, m_time.tm_mday);
      CPPUNIT_ASSERT_EQUAL(6, m_time.tm_wday);
      CPPUNIT_ASSERT_EQUAL(18, m_time.tm_hour);
      CPPUNIT_ASSERT_EQUAL(07, m_time.tm_min);
      CPPUNIT_ASSERT_EQUAL(37, m_time.tm_sec);

      CPPUNIT_ASSERT_EQUAL(1, callbackSetCount());
   }

   void InvalidSetRTCMessageTest()
   {
      char message[MAX_MESSAGE_LENGTH] = {MSG_SET_RTC};
      strncpy(&message[1], "SAT 15-00-01 18:07:37", MAX_MESSAGE_LENGTH); // Bad month (< 1)
      CPPUNIT_ASSERT(!m_message_handler->handle_message(message));
      CPPUNIT_ASSERT(!m_callback_flags[MSG_ID_IDX(MSG_SET_RTC)]);

      strncpy(&message[1], "SAT 15-13-01 18:07:37", MAX_MESSAGE_LENGTH); // Bad month (> 12)
      CPPUNIT_ASSERT(!m_message_handler->handle_message(message));
      CPPUNIT_ASSERT(!m_callback_flags[MSG_ID_IDX(MSG_SET_RTC)]);

      strncpy(&message[1], "SAT 15-08-00 18:07:37", MAX_MESSAGE_LENGTH); // Bad date (< 1)
      CPPUNIT_ASSERT(!m_message_handler->handle_message(message));
      CPPUNIT_ASSERT(!m_callback_flags[MSG_ID_IDX(MSG_SET_RTC)]);

      strncpy(&message[1], "SAT 15-02-29 18:07:37", MAX_MESSAGE_LENGTH); // Bad date (> 28 for February)
      CPPUNIT_ASSERT(!m_message_handler->handle_message(message));
      CPPUNIT_ASSERT(!m_callback_flags[MSG_ID_IDX(MSG_SET_RTC)]);

      strncpy(&message[1], "MAN 15-02-28 18:07:37", MAX_MESSAGE_LENGTH); // Bad day
      CPPUNIT_ASSERT(!m_message_handler->handle_message(message));
      CPPUNIT_ASSERT(!m_callback_flags[MSG_ID_IDX(MSG_SET_RTC)]);

      strncpy(&message[1], "SAT 15-04-31 18:07:37", MAX_MESSAGE_LENGTH); // Bad date (> 30 for April)
      CPPUNIT_ASSERT(!m_message_handler->handle_message(message));
      CPPUNIT_ASSERT(!m_callback_flags[MSG_ID_IDX(MSG_SET_RTC)]);

      strncpy(&message[1], "SAT 15-01-32 18:07:37", MAX_MESSAGE_LENGTH); // Bad date (> 31 for January)
      CPPUNIT_ASSERT(!m_message_handler->handle_message(message));
      CPPUNIT_ASSERT(!m_callback_flags[MSG_ID_IDX(MSG_SET_RTC)]);

      strncpy(&message[1], "SAT 15-01-01 24:07:37", MAX_MESSAGE_LENGTH); // Bad time (> 23 for hour)
      CPPUNIT_ASSERT(!m_message_handler->handle_message(message));
      CPPUNIT_ASSERT(!m_callback_flags[MSG_ID_IDX(MSG_SET_RTC)]);
   
      strncpy(&message[1], "SAT 15-01-01 18:60:37", MAX_MESSAGE_LENGTH); // Bad time (> 59 for minute)
      CPPUNIT_ASSERT(!m_message_handler->handle_message(message));
      CPPUNIT_ASSERT(!m_callback_flags[MSG_ID_IDX(MSG_SET_RTC)]);

      strncpy(&message[1], "SAT 15-01-01 18:07:60", MAX_MESSAGE_LENGTH); // Bad time (> 59 for second)
      CPPUNIT_ASSERT(!m_message_handler->handle_message(message));
      CPPUNIT_ASSERT(!m_callback_flags[MSG_ID_IDX(MSG_SET_RTC)]);

      CPPUNIT_ASSERT_EQUAL(0, callbackSetCount());
   }

   void GetRTCMessageTest()
   {
      m_reply[0] = '\0';
      char message[MAX_MESSAGE_LENGTH] = {MSG_GET_RTC};
      CPPUNIT_ASSERT(m_message_handler->handle_message(message));

      CPPUNIT_ASSERT(m_callback_flags[MSG_ID_IDX(MSG_REPLY)]);
      CPPUNIT_ASSERT_EQUAL(1, callbackSetCount());
        
      std::string expected = std::string("  TUE 13-05-21 17:42:23");
      expected[0] = MSG_REPLY;
      expected[1] = MSG_GET_RTC;

      CPPUNIT_ASSERT_EQUAL(expected, m_reply);
      CPPUNIT_ASSERT_EQUAL(23, (int)m_reply.length());
   }

   void SetAlarmMessageTestWithNoMessage()
   {
      char message[MAX_MESSAGE_LENGTH] = {MSG_SET_ALARM};
      strncpy(&message[1], "", MAX_MESSAGE_LENGTH);
      CPPUNIT_ASSERT(!m_message_handler->handle_message(message));
      CPPUNIT_ASSERT_EQUAL(0, callbackSetCount());
      CPPUNIT_ASSERT(!m_callback_flags[MSG_ID_IDX(MSG_SET_ALARM)]);
   }

   void SetAlarmMessageTestWithWeeklyInterval()
   {
      // Special case with different format to other intervals
      char message[MAX_MESSAGE_LENGTH] = {MSG_SET_ALARM};
      strncpy(&message[1], "01 01W TUE 03:45", MAX_MESSAGE_LENGTH);
      CPPUNIT_ASSERT(m_message_handler->handle_message(message));

      CPPUNIT_ASSERT_EQUAL(1, m_alarm_id);
      CPPUNIT_ASSERT_EQUAL(1, m_alarm.repeat);
      CPPUNIT_ASSERT_EQUAL((int)TUE, m_alarm.datetime.tm_wday);
      CPPUNIT_ASSERT_EQUAL(3, m_alarm.datetime.tm_hour);
      CPPUNIT_ASSERT_EQUAL(45, m_alarm.datetime.tm_min);
      CPPUNIT_ASSERT_EQUAL(60, m_alarm.duration); // Duration defaults to 60 minutes

      CPPUNIT_ASSERT_EQUAL(1, callbackSetCount());
      CPPUNIT_ASSERT(m_callback_flags[MSG_ID_IDX(MSG_SET_ALARM)]);

      // Check that duration still defaults to 60 minutes when string ends with 'D'
      strncpy(&message[1], "01 01W WED", MAX_MESSAGE_LENGTH);
      CPPUNIT_ASSERT(m_message_handler->handle_message(message));

      CPPUNIT_ASSERT_EQUAL(1, m_alarm_id);
      CPPUNIT_ASSERT_EQUAL(1, m_alarm.repeat);
      CPPUNIT_ASSERT_EQUAL((int)WED, m_alarm.datetime.tm_wday);
      CPPUNIT_ASSERT_EQUAL(0, m_alarm.datetime.tm_hour);
      CPPUNIT_ASSERT_EQUAL(0, m_alarm.datetime.tm_min);
      CPPUNIT_ASSERT_EQUAL(60, m_alarm.duration); // Duration defaults to 60 minutes
   }

   void SetAlarmMessageTestWithFullDatetime()
   {
      char message[MAX_MESSAGE_LENGTH] = {MSG_SET_ALARM};
      strncpy(&message[1], "01 01Y 10-09 03:45", MAX_MESSAGE_LENGTH);
      CPPUNIT_ASSERT(m_message_handler->handle_message(message));


      CPPUNIT_ASSERT_EQUAL(1, m_alarm_id);
      CPPUNIT_ASSERT_EQUAL(1, m_alarm.repeat);
      CPPUNIT_ASSERT_EQUAL(INTERVAL_YEAR, m_alarm.repeat_interval);
      CPPUNIT_ASSERT_EQUAL(60, m_alarm.duration); // Duration defaults to 60 minutes

      CPPUNIT_ASSERT_EQUAL(9, m_alarm.datetime.tm_mon); // Month from 0 to 11
      CPPUNIT_ASSERT_EQUAL(9, m_alarm.datetime.tm_mday);
      CPPUNIT_ASSERT_EQUAL(3, m_alarm.datetime.tm_hour);
      CPPUNIT_ASSERT_EQUAL(45, m_alarm.datetime.tm_min);

      CPPUNIT_ASSERT_EQUAL(1, callbackSetCount());
      CPPUNIT_ASSERT(m_callback_flags[MSG_ID_IDX(MSG_SET_ALARM)]);
   }

   void SetAlarmMessageTestWithDuration()
   {
      char message[MAX_MESSAGE_LENGTH] = {MSG_SET_ALARM};
      strncpy(&message[1], "01 01Y 10-09 03:45 D1440", MAX_MESSAGE_LENGTH);
      CPPUNIT_ASSERT(m_message_handler->handle_message(message));

      CPPUNIT_ASSERT_EQUAL(1, m_alarm_id);
      CPPUNIT_ASSERT_EQUAL(1, m_alarm.repeat);
      CPPUNIT_ASSERT_EQUAL(INTERVAL_YEAR, m_alarm.repeat_interval);
      CPPUNIT_ASSERT_EQUAL(1440, m_alarm.duration);

      CPPUNIT_ASSERT_EQUAL(9, m_alarm.datetime.tm_mon); // Month from 0 to 11
      CPPUNIT_ASSERT_EQUAL(9, m_alarm.datetime.tm_mday);
      CPPUNIT_ASSERT_EQUAL(3, m_alarm.datetime.tm_hour);
      CPPUNIT_ASSERT_EQUAL(45, m_alarm.datetime.tm_min);

      CPPUNIT_ASSERT_EQUAL(1, callbackSetCount());
      CPPUNIT_ASSERT(m_callback_flags[MSG_ID_IDX(MSG_SET_ALARM)]);
   }

   void SetAlarmMessageTestWithPartialDatetime()
   {
      char message[MAX_MESSAGE_LENGTH] = {MSG_SET_ALARM};
      strncpy(&message[1], "01 01Y 10-09", MAX_MESSAGE_LENGTH);
      CPPUNIT_ASSERT(m_message_handler->handle_message(message));


      CPPUNIT_ASSERT_EQUAL(1, m_alarm_id);
      CPPUNIT_ASSERT_EQUAL(1, m_alarm.repeat);
      CPPUNIT_ASSERT_EQUAL(9, m_alarm.datetime.tm_mon); // Month from 0 to 11
      CPPUNIT_ASSERT_EQUAL(9, m_alarm.datetime.tm_mday);
      CPPUNIT_ASSERT_EQUAL(0, m_alarm.datetime.tm_hour); // Hour and minute should default to 00:00
      CPPUNIT_ASSERT_EQUAL(0, m_alarm.datetime.tm_min);

      CPPUNIT_ASSERT_EQUAL(1, callbackSetCount());
      CPPUNIT_ASSERT(m_callback_flags[MSG_ID_IDX(MSG_SET_ALARM)]);
   }

   void SetAlarmMessageTestWithNoDatetime()
   {
      char message[MAX_MESSAGE_LENGTH] = {MSG_SET_ALARM};
      strncpy(&message[1], "01 01Y", MAX_MESSAGE_LENGTH);
      CPPUNIT_ASSERT(m_message_handler->handle_message(message));


      CPPUNIT_ASSERT_EQUAL(1, m_alarm_id);
      CPPUNIT_ASSERT_EQUAL(1, m_alarm.repeat);
      // Date should default to 1st January 00:00
      CPPUNIT_ASSERT_EQUAL(0, m_alarm.datetime.tm_mon); // Month from 0 to 11
      CPPUNIT_ASSERT_EQUAL(1, m_alarm.datetime.tm_mday);
      CPPUNIT_ASSERT_EQUAL(0, m_alarm.datetime.tm_hour); 
      CPPUNIT_ASSERT_EQUAL(0, m_alarm.datetime.tm_min);

      CPPUNIT_ASSERT_EQUAL(1, callbackSetCount());
      CPPUNIT_ASSERT(m_callback_flags[MSG_ID_IDX(MSG_SET_ALARM)]);
   }

   void SetAlarmMessageTestWithBadDatetime()
   {
      char message[MAX_MESSAGE_LENGTH] = {MSG_SET_ALARM};
      strncpy(&message[1], "01 01A", MAX_MESSAGE_LENGTH); // Bad interval setting
      CPPUNIT_ASSERT(!m_message_handler->handle_message(message));

      strncpy(&message[1], "01 01Y 13", MAX_MESSAGE_LENGTH); // Bad month setting
      CPPUNIT_ASSERT(!m_message_handler->handle_message(message));

      strncpy(&message[1], "01 01Y 12-32", MAX_MESSAGE_LENGTH); // Bad date setting
      CPPUNIT_ASSERT(!m_message_handler->handle_message(message));

      strncpy(&message[1], "01 01Y 12-31 24:00", MAX_MESSAGE_LENGTH); // Bad hour setting
      CPPUNIT_ASSERT(!m_message_handler->handle_message(message));
          
      strncpy(&message[1], "01 01Y 12-31 23:60", MAX_MESSAGE_LENGTH); // Bad minute setting
      CPPUNIT_ASSERT(!m_message_handler->handle_message(message));

      strncpy(&message[1], "01 01Y 12/31 2359", MAX_MESSAGE_LENGTH); // Malformed datetime string
      CPPUNIT_ASSERT(!m_message_handler->handle_message(message));

      CPPUNIT_ASSERT_EQUAL(0, callbackSetCount());
      CPPUNIT_ASSERT(!m_callback_flags[MSG_ID_IDX(MSG_SET_ALARM)]);
   }

   void SetAlarmMessageTestActionID()
   {
      char message[MAX_MESSAGE_LENGTH] = {MSG_SET_ALARM};

      strncpy(&message[1], "02 01Y", MAX_MESSAGE_LENGTH);
      CPPUNIT_ASSERT(m_message_handler->handle_message(message));
      CPPUNIT_ASSERT_EQUAL(2, m_alarm_id);

      strncpy(&message[1], "03 01Y", MAX_MESSAGE_LENGTH);
      CPPUNIT_ASSERT(m_message_handler->handle_message(message));
      CPPUNIT_ASSERT_EQUAL(3, m_alarm_id);

      strncpy(&message[1], "16 01Y", MAX_MESSAGE_LENGTH);
      CPPUNIT_ASSERT(m_message_handler->handle_message(message));
      CPPUNIT_ASSERT_EQUAL(16, m_alarm_id);

      CPPUNIT_ASSERT_EQUAL(1, callbackSetCount());
      CPPUNIT_ASSERT(m_callback_flags[MSG_ID_IDX(MSG_SET_ALARM)]);

      // Invalid action ID
      m_callback_flags[MSG_ID_IDX(MSG_SET_ALARM)] = false;
      strncpy(&message[1], "17 01Y", MAX_MESSAGE_LENGTH);
      CPPUNIT_ASSERT(!m_message_handler->handle_message(message));
      CPPUNIT_ASSERT_EQUAL(16, m_alarm_id);
      CPPUNIT_ASSERT(!m_callback_flags[MSG_ID_IDX(MSG_SET_ALARM)]);
   }

   void SetAlarmMessageTestRepeatCount()
   {
      char message[MAX_MESSAGE_LENGTH] = {MSG_SET_ALARM};

      strncpy(&message[1], "01 01Y", MAX_MESSAGE_LENGTH);
      CPPUNIT_ASSERT(m_message_handler->handle_message(message));
      CPPUNIT_ASSERT_EQUAL(1, m_alarm.repeat);
 
      strncpy(&message[1], "01 02Y", MAX_MESSAGE_LENGTH);
      CPPUNIT_ASSERT(m_message_handler->handle_message(message));
      CPPUNIT_ASSERT_EQUAL(2, m_alarm.repeat);

      strncpy(&message[1], "01 50Y", MAX_MESSAGE_LENGTH);
      CPPUNIT_ASSERT(m_message_handler->handle_message(message));
      CPPUNIT_ASSERT_EQUAL(50, m_alarm.repeat);

      CPPUNIT_ASSERT_EQUAL(1, callbackSetCount());
      CPPUNIT_ASSERT(m_callback_flags[MSG_ID_IDX(MSG_SET_ALARM)]);

      // Invalid repeat count
      m_callback_flags[MSG_ID_IDX(MSG_SET_ALARM)] = false;
      strncpy(&message[1], "01 51Y", MAX_MESSAGE_LENGTH);
      CPPUNIT_ASSERT(!m_message_handler->handle_message(message));
      CPPUNIT_ASSERT(!m_callback_flags[MSG_ID_IDX(MSG_SET_ALARM)]);
   }

   void ClrAlarmMessageTest()
   {
      char message[MAX_MESSAGE_LENGTH] = {MSG_CLEAR_ALARM};
      strncpy(&message[1], "01", MAX_MESSAGE_LENGTH);

      CPPUNIT_ASSERT(m_message_handler->handle_message(message));
      CPPUNIT_ASSERT_EQUAL(1, m_alarm_id);

      CPPUNIT_ASSERT_EQUAL(1, callbackSetCount());
      CPPUNIT_ASSERT(m_callback_flags[MSG_ID_IDX(MSG_CLEAR_ALARM)]);

      strncpy(&message[1], "02", MAX_MESSAGE_LENGTH);
      CPPUNIT_ASSERT(m_message_handler->handle_message(message));
      CPPUNIT_ASSERT_EQUAL(2, m_alarm_id);

      strncpy(&message[1], "16", MAX_MESSAGE_LENGTH);
      CPPUNIT_ASSERT(m_message_handler->handle_message(message));
      CPPUNIT_ASSERT_EQUAL(16, m_alarm_id);

      CPPUNIT_ASSERT_EQUAL(1, callbackSetCount());
      CPPUNIT_ASSERT(m_callback_flags[MSG_ID_IDX(MSG_CLEAR_ALARM)]);

      m_callback_flags[MSG_ID_IDX(MSG_CLEAR_ALARM)] = false;
      strncpy(&message[1], "17", MAX_MESSAGE_LENGTH); // Invalid action ID
      CPPUNIT_ASSERT_EQUAL(16, m_alarm_id);
      CPPUNIT_ASSERT(!m_message_handler->handle_message(message));
      CPPUNIT_ASSERT(!m_callback_flags[MSG_ID_IDX(MSG_CLEAR_ALARM)]);
   }

   void SetTriggerMessageTest()
   {
      char message[MAX_MESSAGE_LENGTH] = {MSG_SET_TRIGGER};
      strncpy(&message[1], "0 1&2|A1", MAX_MESSAGE_LENGTH);
      
      CPPUNIT_ASSERT(m_message_handler->handle_message(message));

      std::string expected = std::string("1&2|A1");
      CPPUNIT_ASSERT_EQUAL(expected, m_reply);

      CPPUNIT_ASSERT_EQUAL(1, callbackSetCount());
      CPPUNIT_ASSERT(m_callback_flags[MSG_ID_IDX(MSG_SET_TRIGGER)]);
   }

   void ClearTriggerMessageTest()
   {
      char message[MAX_MESSAGE_LENGTH] = {MSG_CLEAR_TRIGGER};
      strncpy(&message[1], "0", MAX_MESSAGE_LENGTH);
      
      CPPUNIT_ASSERT(m_message_handler->handle_message(message));
      CPPUNIT_ASSERT_EQUAL(1, callbackSetCount());
      CPPUNIT_ASSERT(m_callback_flags[MSG_ID_IDX(MSG_CLEAR_TRIGGER)]);
   }

   void SetIOTypeMessageTest()
   {
      char message[MAX_MESSAGE_LENGTH] = {MSG_SET_IO_TYPE};
      strncpy(&message[1], "1 OUT", MAX_MESSAGE_LENGTH);

      CPPUNIT_ASSERT(m_message_handler->handle_message(message));
      CPPUNIT_ASSERT_EQUAL(OUTPUT, m_io_type);
      CPPUNIT_ASSERT_EQUAL(1, m_io_index);

      strncpy(&message[1], "2 IN", MAX_MESSAGE_LENGTH);
      CPPUNIT_ASSERT(m_message_handler->handle_message(message));
      CPPUNIT_ASSERT_EQUAL(INPUT, m_io_type);
      CPPUNIT_ASSERT_EQUAL(2, m_io_index);

      CPPUNIT_ASSERT_EQUAL(1, callbackSetCount());
      CPPUNIT_ASSERT(m_callback_flags[MSG_ID_IDX(MSG_SET_IO_TYPE)]);

      m_callback_flags[MSG_ID_IDX(MSG_SET_IO_TYPE)] = false;

      strncpy(&message[1], "4 IN", MAX_MESSAGE_LENGTH); // Invalid IO index
      CPPUNIT_ASSERT(!m_message_handler->handle_message(message));

      strncpy(&message[1], "1 XX", MAX_MESSAGE_LENGTH); // Invalid io type
      CPPUNIT_ASSERT(!m_message_handler->handle_message(message));

      CPPUNIT_ASSERT(!m_callback_flags[MSG_ID_IDX(MSG_SET_IO_TYPE)]);
   }

   void ReadInputMessageTest()
   {
      char message[MAX_MESSAGE_LENGTH] = {MSG_READ_INPUT};
      
      strncpy(&message[1], "0", MAX_MESSAGE_LENGTH);
      CPPUNIT_ASSERT(m_message_handler->handle_message(message));

      CPPUNIT_ASSERT_EQUAL((char)MSG_REPLY, m_reply[0]);
      CPPUNIT_ASSERT_EQUAL((char)MSG_READ_INPUT, m_reply[1]);
      CPPUNIT_ASSERT_EQUAL('1', m_reply[2]);
      CPPUNIT_ASSERT(m_callback_flags[MSG_ID_IDX(MSG_REPLY)]);

      strncpy(&message[1], "1", MAX_MESSAGE_LENGTH);
      CPPUNIT_ASSERT(m_message_handler->handle_message(message));
      CPPUNIT_ASSERT_EQUAL((char)MSG_REPLY, m_reply[0]);
      CPPUNIT_ASSERT_EQUAL((char)MSG_READ_INPUT, m_reply[1]);
      CPPUNIT_ASSERT_EQUAL('0', m_reply[2]);

      strncpy(&message[1], "2", MAX_MESSAGE_LENGTH);
      CPPUNIT_ASSERT(m_message_handler->handle_message(message));
      CPPUNIT_ASSERT_EQUAL((char)MSG_REPLY, m_reply[0]);
      CPPUNIT_ASSERT_EQUAL((char)MSG_READ_INPUT, m_reply[1]);
      CPPUNIT_ASSERT_EQUAL('0', m_reply[2]);

      strncpy(&message[1], "3", MAX_MESSAGE_LENGTH);
      CPPUNIT_ASSERT(m_message_handler->handle_message(message));
      CPPUNIT_ASSERT_EQUAL((char)MSG_REPLY, m_reply[0]);
      CPPUNIT_ASSERT_EQUAL((char)MSG_READ_INPUT, m_reply[1]);
      CPPUNIT_ASSERT_EQUAL('1', m_reply[2]);

      CPPUNIT_ASSERT_EQUAL(1, callbackSetCount());

      m_callback_flags[MSG_ID_IDX(MSG_REPLY)] = false;
      strncpy(&message[1], "4", MAX_MESSAGE_LENGTH);
      CPPUNIT_ASSERT(!m_message_handler->handle_message(message));
      CPPUNIT_ASSERT(!m_callback_flags[MSG_ID_IDX(MSG_REPLY)]);
   }

   void ResetMessageTest()
   {
      char message[MAX_MESSAGE_LENGTH] = {MSG_RESET};
      CPPUNIT_ASSERT(m_message_handler->handle_message(message));

      CPPUNIT_ASSERT(m_callback_flags[MSG_ID_IDX(MSG_RESET)]);
      CPPUNIT_ASSERT_EQUAL(1, callbackSetCount());
   }
};


static MessagingTest * s_test_object;

static void set_test_object(void* obj) { s_test_object = (MessagingTest *)obj; }

static bool set_rtc_callback(TM*tm)
{
   return s_test_object->Set_rtc_callback(tm);
}

static bool set_alarm_callback(int alarm_id, ALARM * pAlarm)
{
   return s_test_object->Set_alarm_callback(alarm_id, pAlarm);
}

static bool clr_alarm_callback(int alarm_id)
{
   return s_test_object->Clr_alarm_callback(alarm_id);
}

static bool set_trigger_callback(int io_index, char * pTriggerExpression)
{
   return s_test_object->Set_trigger_callback(io_index, pTriggerExpression);
}

static bool clear_trigger_callback(int io_index)
{
   return s_test_object->Clear_trigger_callback(io_index);
}

static bool set_io_type_callback(int io_index, IO_TYPE io_type)
{
   return s_test_object->Set_io_type_callback(io_index, io_type);
}

static bool reset_callback(void)
{
   return s_test_object->Reset_callback();
}

static bool reply_callback(char * message)
{
   return s_test_object->Reply_callback(message);
}

int main()
{
   CppUnit::TextUi::TestRunner runner;
   
   CPPUNIT_TEST_SUITE_REGISTRATION( MessagingTest );

   CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();

   runner.addTest( registry.makeTest() );
   runner.run();

   return 0;
}