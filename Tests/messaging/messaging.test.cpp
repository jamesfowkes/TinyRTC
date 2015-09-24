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
#include <cppunit/TestAssert.h>

#include "Utility/util_time.h"

#include "alarm.h"
#include "io.h"
#include "messaging.h"
#include "ast_node.h"
#include "syntax_parser.h"

#include "app.io.h"

static bool set_rtc_callback(TM* tm);
static bool set_alarm_callback(int alarm_id, Alarm * pAlarm);
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
   CPPUNIT_TEST(InvalidSetRTCMessageTestMonthLessThanOne);
   CPPUNIT_TEST(InvalidSetRTCMessageTestMonthMoreThanTwelve);
   CPPUNIT_TEST(InvalidSetRTCMessageTestDateLessThanOne);
   CPPUNIT_TEST(InvalidSetRTCMessageTestDateExceedsMaximumForFeb);
   CPPUNIT_TEST(InvalidSetRTCMessageTestExceedsMaximumfor30DayMonth);
   CPPUNIT_TEST(InvalidSetRTCMessageTestExceedsMaximumfor31DayMonth);
   CPPUNIT_TEST(InvalidSetRTCMessageTestDayDoesNotExist);
   CPPUNIT_TEST(InvalidSetRTCMessageTestHourGreaterThan23);
   CPPUNIT_TEST(InvalidSetRTCMessageTestMinuteGreaterThan59);
   CPPUNIT_TEST(InvalidSetRTCMessageTestSecondGreaterThan59);
   CPPUNIT_TEST(GetRTCMessageTest);
   CPPUNIT_TEST(SetAlarmMessageTestWithNoMessage);
   CPPUNIT_TEST(SetAlarmMessageTestWithWeeklyInterval);
   CPPUNIT_TEST(SetAlarmMessageTestWithWeeklyIntervalOnWedDoesNotFailOnDuration);
   CPPUNIT_TEST(SetAlarmMessageTestWithFullDatetime);
   CPPUNIT_TEST(SetAlarmMessageTestWithDuration);
   CPPUNIT_TEST(SetAlarmMessageTestWithPartialDatetime);
   CPPUNIT_TEST(SetAlarmMessageTestWithNoDatetime);
   CPPUNIT_TEST(SetAlarmMessageTestWithBadIntervalSetting);
   CPPUNIT_TEST(SetAlarmMessageTestWithBadMonthSetting);
   CPPUNIT_TEST(SetAlarmMessageTestWithBadDateSetting);
   CPPUNIT_TEST(SetAlarmMessageTestWithBadHourSetting);
   CPPUNIT_TEST(SetAlarmMessageTestWithBadMinuteSetting);
   CPPUNIT_TEST(SetAlarmMessageTestWithMalformedSetting);
   CPPUNIT_TEST(SetAlarmMessageTestActionID);
   CPPUNIT_TEST(SetAlarmMessageTestInvalidActionID);
   CPPUNIT_TEST(SetAlarmMessageTestRepeatCount);
   CPPUNIT_TEST(SetAlarmMessageTestInvalidRepeatCount);
   CPPUNIT_TEST(ClrAlarmValidMessageTest);
   CPPUNIT_TEST(ClrAlarmInvalidMessageTest);
   CPPUNIT_TEST(SetTriggerMessageTest);
   CPPUNIT_TEST(ClearTriggerMessageTest);
   CPPUNIT_TEST(SetIOTypeMessageTest);
   CPPUNIT_TEST(SetIOTypeInvalidMessageTest);
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

   bool Set_alarm_callback(int alarm_id, Alarm * pAlarm)
   {
      m_callback_flags[MSG_ID_IDX(MSG_SET_ALARM)] = true;
      m_alarm = *pAlarm;
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

      m_reply[0] = '\0';

      m_alarm.reset();
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
   
   Alarm m_alarm;
   int m_alarm_id;

   IO_TYPE m_io_type;
   int m_io_index;

   int m_io_trigger;

   char m_message[MAX_MESSAGE_LENGTH];
   MessageHandler * m_message_handler;

   MSG_HANDLER_FUNCTIONS m_callbacks;

   int callback_set_count()
   {
      uint8_t count = 0;

      for(int i = 0; i < MSG_MAX_ID; ++i)
      {
         if (m_callback_flags[i]) { count++; }
      }

      return count;
   }

   void build_message(char id, char const * pMessageBody)
   {
      m_message[0] = id;
      strncpy(&m_message[1], pMessageBody, MAX_MESSAGE_LENGTH-1);
   }

   void assert_valid_reply(char id)
   {
      std::ostringstream os;
      os << (char)MSG_REPLY << id << " OK";
      std::string expected = os.str();
      CPPUNIT_ASSERT_EQUAL(expected, m_reply);
   }

   void assert_invalid_reply(char id)
   {
      std::ostringstream os;
      os << (char)MSG_REPLY << id << " FAIL";
      std::string expected = os.str();
      CPPUNIT_ASSERT_EQUAL(expected, m_reply);
   }

   void clear_reply() { m_reply[0] = '\0'; }

   void assert_message_passes_on_handling(bool message_callback_expected, std::string * expected_reply = NULL)
   {
      char id = m_message[0];
      CPPUNIT_ASSERT(m_message_handler->handle_message(m_message));
      if (message_callback_expected)
      {
         CPPUNIT_ASSERT_EQUAL(1, callback_set_count());
         CPPUNIT_ASSERT(m_callback_flags[MSG_ID_IDX(id)]);
      }
      else
      {
         CPPUNIT_ASSERT_EQUAL(0, callback_set_count());
         CPPUNIT_ASSERT(!m_callback_flags[MSG_ID_IDX(id)]);  
      }

      if (expected_reply)
      {
         CPPUNIT_ASSERT_EQUAL(*expected_reply, m_reply);
      }
      else
      {
         assert_valid_reply(id);
      }
   }

   void assert_message_fails_on_handling()
   {
      char id = m_message[0];
      CPPUNIT_ASSERT(!m_message_handler->handle_message(m_message));
      CPPUNIT_ASSERT_EQUAL(0, callback_set_count());
      CPPUNIT_ASSERT(!m_callback_flags[MSG_ID_IDX(id)]);
      assert_invalid_reply(id);
   }

protected:

   void ValidSetRTCMessageTest()
   {
      build_message(MSG_SET_RTC, "SAT 15-08-01 18:07:37");
      assert_message_passes_on_handling(true);

      CPPUNIT_ASSERT_EQUAL(15, m_time.tm_year);
      CPPUNIT_ASSERT_EQUAL(7, m_time.tm_mon); // Month from 0 to 11
      CPPUNIT_ASSERT_EQUAL(1, m_time.tm_mday);
      CPPUNIT_ASSERT_EQUAL(6, m_time.tm_wday);
      CPPUNIT_ASSERT_EQUAL(18, m_time.tm_hour);
      CPPUNIT_ASSERT_EQUAL(07, m_time.tm_min);
      CPPUNIT_ASSERT_EQUAL(37, m_time.tm_sec);
   }

   void InvalidSetRTCMessageTestMonthLessThanOne()
   {
      build_message(MSG_SET_RTC, "SAT 15-00-01 18:07:37");
      assert_message_fails_on_handling();
   }

   void InvalidSetRTCMessageTestMonthMoreThanTwelve()
   {
      build_message(MSG_SET_RTC, "SAT 15-13-01 18:07:37");
      assert_message_fails_on_handling();
   }

   void InvalidSetRTCMessageTestDateLessThanOne()
   {
      build_message(MSG_SET_RTC, "SAT 15-08-00 18:07:37");
      assert_message_fails_on_handling();
   }
   
   void InvalidSetRTCMessageTestDateExceedsMaximumForFeb()
   {
      build_message(MSG_SET_RTC, "SAT 15-08-00 18:07:37");
      assert_message_fails_on_handling();
   }

   void InvalidSetRTCMessageTestExceedsMaximumfor30DayMonth()
   {
      build_message(MSG_SET_RTC, "SAT 15-04-31 18:07:37");
      assert_message_fails_on_handling();
   }

   void InvalidSetRTCMessageTestExceedsMaximumfor31DayMonth()
   {
      build_message(MSG_SET_RTC, "SAT 15-01-32 18:07:37");
      assert_message_fails_on_handling();
   }

   void InvalidSetRTCMessageTestDayDoesNotExist()
   {
      build_message(MSG_SET_RTC, "??? 15-02-28 18:07:37");
      assert_message_fails_on_handling();
   }

   void InvalidSetRTCMessageTestHourGreaterThan23()
   {
      build_message(MSG_SET_RTC, "SAT 15-01-01 24:07:37");
      assert_message_fails_on_handling();
   }

   void InvalidSetRTCMessageTestMinuteGreaterThan59()
   {
      build_message(MSG_SET_RTC, "SAT 15-01-01 18:60:37");
      assert_message_fails_on_handling();
   }

   void InvalidSetRTCMessageTestSecondGreaterThan59()
   {
      build_message(MSG_SET_RTC, "SAT 15-01-01 18:07:60");
      assert_message_fails_on_handling();
   }

   void GetRTCMessageTest()
   {
      build_message(MSG_GET_RTC, "");
      std::string expected = std::string("  TUE 13-05-21 17:42:23");
      expected[0] = MSG_REPLY;
      expected[1] = MSG_GET_RTC;
      assert_message_passes_on_handling(false, &expected);

      CPPUNIT_ASSERT_EQUAL(23, (int)m_reply.length());
   }

   void SetAlarmMessageTestWithNoMessage()
   {
      build_message(MSG_SET_ALARM, "");
      assert_message_fails_on_handling();
   }

   void SetAlarmMessageTestWithWeeklyInterval()
   {
      // Special case with different format to other intervals
      build_message(MSG_SET_ALARM, "01 01W TUE 03:45");
      assert_message_passes_on_handling(true);
      
      TM expected_time; set_default_alarm_time(&expected_time);
      expected_time.tm_hour = 3;
      expected_time.tm_min = 45;
      expected_time.tm_wday = TUE;
      Alarm expected_alarm = Alarm((INTERVAL)'W', &expected_time, 1, 60);

      CPPUNIT_ASSERT_EQUAL(1, m_alarm_id);
      CPPUNIT_ASSERT_EQUAL(expected_alarm, m_alarm);
   }

   void SetAlarmMessageTestWithWeeklyIntervalOnWedDoesNotFailOnDuration()
   {
      // Check that duration still defaults to 60 minutes when string ends with 'D'
      build_message(MSG_SET_ALARM, "01 01W WED");
      assert_message_passes_on_handling(true);

      TM expected_time; set_default_alarm_time(&expected_time);
      expected_time.tm_wday = WED;

      Alarm expected_alarm = Alarm((INTERVAL)'W', &expected_time, 1, 60);

      CPPUNIT_ASSERT_EQUAL(1, m_alarm_id);
      CPPUNIT_ASSERT_EQUAL(expected_alarm, m_alarm);     
   }

   void SetAlarmMessageTestWithFullDatetime()
   {
      build_message(MSG_SET_ALARM, "01 01Y 10-09 03:45");
      assert_message_passes_on_handling(true);

      TM expected_time; set_default_alarm_time(&expected_time);
      expected_time.tm_mon = OCT;
      expected_time.tm_mday = 9;
      expected_time.tm_hour = 3;
      expected_time.tm_min = 45;

      Alarm expected_alarm = Alarm((INTERVAL)'Y', &expected_time, 1, 60);
      
      CPPUNIT_ASSERT_EQUAL(1, m_alarm_id);
      CPPUNIT_ASSERT_EQUAL(expected_alarm, m_alarm);
   }

   void SetAlarmMessageTestWithDuration()
   {
      build_message(MSG_SET_ALARM, "01 01Y 10-09 03:45 D1440");
      assert_message_passes_on_handling(true);

      TM expected_time; set_default_alarm_time(&expected_time);
      expected_time.tm_mon = OCT;
      expected_time.tm_mday = 9;
      expected_time.tm_hour = 3;
      expected_time.tm_min = 45;

      Alarm expected_alarm = Alarm((INTERVAL)'Y', &expected_time, 1, 1440);
      
      CPPUNIT_ASSERT_EQUAL(1, m_alarm_id);
      CPPUNIT_ASSERT_EQUAL(expected_alarm, m_alarm);
   }

   void SetAlarmMessageTestWithPartialDatetime()
   {
      build_message(MSG_SET_ALARM, "01 01Y 10-09");
      assert_message_passes_on_handling(true);

      TM expected_time; set_default_alarm_time(&expected_time);
      expected_time.tm_mon = OCT;
      expected_time.tm_mday = 9;
      Alarm expected_alarm = Alarm((INTERVAL)'Y', &expected_time, 1, 60);
      
      CPPUNIT_ASSERT_EQUAL(1, m_alarm_id);
      CPPUNIT_ASSERT_EQUAL(expected_alarm, m_alarm);
   }

   void SetAlarmMessageTestWithNoDatetime()
   {
      build_message(MSG_SET_ALARM, "01 01Y");
      assert_message_passes_on_handling(true);

      TM expected_time; set_default_alarm_time(&expected_time);
      Alarm expected_alarm = Alarm((INTERVAL)'Y', &expected_time, 1, 60);
      
      CPPUNIT_ASSERT_EQUAL(1, m_alarm_id);
      CPPUNIT_ASSERT_EQUAL(expected_alarm, m_alarm);
   }

   void SetAlarmMessageTestWithBadIntervalSetting()
   {
      build_message(MSG_SET_ALARM, "01 01A");
      assert_message_fails_on_handling();
   }

   void SetAlarmMessageTestWithBadMonthSetting()
   {
      build_message(MSG_SET_ALARM, "01 01Y 13");
      assert_message_fails_on_handling();
   }

   void SetAlarmMessageTestWithBadDateSetting()
   {
      build_message(MSG_SET_ALARM, "01 01Y 12-32");
      assert_message_fails_on_handling();
   }

   void SetAlarmMessageTestWithBadHourSetting()
   {      
      build_message(MSG_SET_ALARM, "01 01Y 12-31 24:00");
      assert_message_fails_on_handling();
   }

   void SetAlarmMessageTestWithBadMinuteSetting()
   {
      build_message(MSG_SET_ALARM, "01 01Y 12-31 23:60");
      assert_message_fails_on_handling();
   }

   void SetAlarmMessageTestWithMalformedSetting()
   {
      build_message(MSG_SET_ALARM, "01 01Y 12/31 2359");
      assert_message_fails_on_handling();
   }

   void SetAlarmMessageTestActionID()
   {      
      build_message(MSG_SET_ALARM, "01 01Y");
      assert_message_passes_on_handling(true);

      CPPUNIT_ASSERT_EQUAL(1, m_alarm_id);
      
      clear_reply();
      build_message(MSG_SET_ALARM, "06 01Y");
      assert_message_passes_on_handling(true);

      CPPUNIT_ASSERT_EQUAL(6, m_alarm_id);
   }

   void SetAlarmMessageTestInvalidActionID()
   {
      build_message(MSG_SET_ALARM, "17 01Y");
      assert_message_fails_on_handling();
      CPPUNIT_ASSERT_EQUAL(-1, m_alarm_id);
   }

   void SetAlarmMessageTestRepeatCount()
   {
      build_message(MSG_SET_ALARM, "01 01Y");
      assert_message_passes_on_handling(true);
      CPPUNIT_ASSERT_EQUAL(1, m_alarm.get_repeat());
 
      build_message(MSG_SET_ALARM, "01 02Y");
      assert_message_passes_on_handling(true);
      CPPUNIT_ASSERT_EQUAL(2, m_alarm.get_repeat());

      build_message(MSG_SET_ALARM, "01 50Y");
      assert_message_passes_on_handling(true);
      CPPUNIT_ASSERT_EQUAL(50, m_alarm.get_repeat());
   }

   void SetAlarmMessageTestInvalidRepeatCount()
   {
      build_message(MSG_SET_ALARM, "01 51Y");
      assert_message_fails_on_handling();
   }

   void ClrAlarmValidMessageTest()
   {
      build_message(MSG_CLEAR_ALARM, "01");
      assert_message_passes_on_handling(true);
      CPPUNIT_ASSERT_EQUAL(1, m_alarm_id);

      build_message(MSG_CLEAR_ALARM, "02");
      assert_message_passes_on_handling(true);
      CPPUNIT_ASSERT_EQUAL(2, m_alarm_id);

      build_message(MSG_CLEAR_ALARM, "16");
      assert_message_passes_on_handling(true);
      CPPUNIT_ASSERT_EQUAL(16, m_alarm_id);
   }

   void ClrAlarmInvalidMessageTest()
   {
      build_message(MSG_CLEAR_ALARM, "17");
      assert_message_fails_on_handling();
      CPPUNIT_ASSERT_EQUAL(-1, m_alarm_id);
   }

   void SetTriggerMessageTest()
   {
      build_message(MSG_SET_TRIGGER, "0 1&2|A1");
      assert_message_passes_on_handling(true);
   }

   void ClearTriggerMessageTest()
   {
      build_message(MSG_CLEAR_TRIGGER, "0");
      assert_message_passes_on_handling(true);
   }

   void SetIOTypeMessageTest()
   {
      build_message(MSG_SET_IO_TYPE, "1 OUT");
      assert_message_passes_on_handling(true);
      CPPUNIT_ASSERT_EQUAL(OUTPUT, m_io_type);
      CPPUNIT_ASSERT_EQUAL(1, m_io_index);

      build_message(MSG_SET_IO_TYPE, "2 IN");
      assert_message_passes_on_handling(true);
      CPPUNIT_ASSERT_EQUAL(INPUT, m_io_type);
      CPPUNIT_ASSERT_EQUAL(2, m_io_index);
   }

   void SetIOTypeInvalidMessageTest()
   {
      build_message(MSG_SET_IO_TYPE, "4 IN");
      assert_message_fails_on_handling();

      build_message(MSG_SET_IO_TYPE, "1 XX");
      assert_message_fails_on_handling();
   }

   void ReadInputMessageTest()
   {
      build_message(MSG_READ_INPUT, "0");
      std::string expected = std::string("  1");
      expected[0] = MSG_REPLY;
      expected[1] = MSG_READ_INPUT;

      assert_message_passes_on_handling(false, &expected);

      build_message(MSG_READ_INPUT, "1");
      expected[2] = '0';
      assert_message_passes_on_handling(false, &expected);

      build_message(MSG_READ_INPUT, "2");
      expected[2] = '0';
      assert_message_passes_on_handling(false, &expected);

      build_message(MSG_READ_INPUT, "3");
      expected[2] = '1';
      assert_message_passes_on_handling(false, &expected);
   }

   void ResetMessageTest()
   {
      build_message(MSG_RESET, "");
      std::string expected = std::string("  RESET");
      expected[0] = MSG_REPLY;
      expected[1] = MSG_RESET;

      assert_message_passes_on_handling(true, &expected);
   }
};


static MessagingTest * s_test_object;

static void set_test_object(void* obj) { s_test_object = (MessagingTest *)obj; }

static bool set_rtc_callback(TM*tm)
{
   return s_test_object->Set_rtc_callback(tm);
}

static bool set_alarm_callback(int alarm_id, Alarm * pAlarm)
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