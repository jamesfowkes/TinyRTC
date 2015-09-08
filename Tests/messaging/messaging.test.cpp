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
#include "messaging.h"
#include "ast_node.h"
#include "syntax_parser.h"

static bool setRTC_callback(TM* tm);
static bool setAlarm_callback(int actionID, ALARM * pAlarm);
static void clrAlarm_callback(void);
static void setIOType_callback(void);
static void readInput_callback(void);
static void reset_callback(void);
static void invalid_callback(void);
static bool reply_callback(char * message);

static void setTestObject(void* obj);

class MessagingTest : public CppUnit::TestFixture  {

   CPPUNIT_TEST_SUITE(MessagingTest);
   CPPUNIT_TEST(ValidSetRTCMessageTest);
   CPPUNIT_TEST(InvalidSetRTCMessageTest);
   CPPUNIT_TEST(GetRTCMessageTest);
   CPPUNIT_TEST(SetAlarmMessageTestWithFullDatetime);
   CPPUNIT_TEST(SetAlarmMessageTestWithPartialDatetime);
   CPPUNIT_TEST(SetAlarmMessageTestWithNoDatetime);
   CPPUNIT_TEST(SetAlarmMessageTestWithBadDatetime);
   CPPUNIT_TEST(SetAlarmMessageTestActionID);
   CPPUNIT_TEST_SUITE_END();

public:

   bool SetRTC_callback(TM* tm)
   {
      m_callback_flags[MSG_ID_IDX(MSG_SET_RTC)] = true;
      time_cpy(&m_time, tm);
      return true;
   }

   bool SetAlarm_callback(int actionID, ALARM * pAlarm)
   {
      m_callback_flags[MSG_ID_IDX(MSG_SET_ALARM)] = true;
      time_cpy(&(m_alarm.datetime), &(pAlarm->datetime));
      m_alarm.repeat = pAlarm->repeat;
      m_alarmID = actionID;
      return true;
   }

   void ClrAlarm_callback(void)
   {
      m_callback_flags[MSG_ID_IDX(MSG_CLEAR_ALARM)] = true;
   }

   void SetIOType_callback(void)
   {
      m_callback_flags[MSG_ID_IDX(MSG_SET_IO_TYPE)] = true;  
   }

   void ReadInput_callback(void)
   {
      m_callback_flags[MSG_ID_IDX(MSG_READ_INPUT)] = true;  
   }

   void Reset_callback(void)
   {
      m_callback_flags[MSG_ID_IDX(MSG_RESET)] = true;  
   }

   void Invalid_callback(void)
   {
      m_callback_flags[MSG_ID_IDX(MSG_INVALID)] = true;  
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
      m_messageHandler = new MessageHandler(&m_callbacks);

      m_callbacks.setRTCfn = setRTC_callback;
      m_callbacks.setAlarmfn = setAlarm_callback;
      m_callbacks.setIOTypefn = setIOType_callback;
      m_callbacks.readInputfn = readInput_callback;
      m_callbacks.resetfn = reset_callback;
      m_callbacks.invalidfn = invalid_callback;
      m_callbacks.replyfn = reply_callback;

      setTestObject(this);
   }

   void tearDown(void)
   {
      free(m_messageHandler);
   }

private:
   
   bool m_callback_flags[MSG_MAX_ID];

   TM m_time;
   std::string m_reply;
   ALARM m_alarm;
   int m_alarmID;

   MessageHandler * m_messageHandler;

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
      CPPUNIT_ASSERT(m_messageHandler->handleMessage(message));
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
      CPPUNIT_ASSERT(!m_messageHandler->handleMessage(message));
      CPPUNIT_ASSERT(!m_callback_flags[MSG_ID_IDX(MSG_SET_RTC)]);

      strncpy(&message[1], "SAT 15-13-01 18:07:37", MAX_MESSAGE_LENGTH); // Bad month (> 12)
      CPPUNIT_ASSERT(!m_messageHandler->handleMessage(message));
      CPPUNIT_ASSERT(!m_callback_flags[MSG_ID_IDX(MSG_SET_RTC)]);

      strncpy(&message[1], "SAT 15-08-00 18:07:37", MAX_MESSAGE_LENGTH); // Bad date (< 1)
      CPPUNIT_ASSERT(!m_messageHandler->handleMessage(message));
      CPPUNIT_ASSERT(!m_callback_flags[MSG_ID_IDX(MSG_SET_RTC)]);

      strncpy(&message[1], "SAT 15-02-29 18:07:37", MAX_MESSAGE_LENGTH); // Bad date (> 28 for February)
      CPPUNIT_ASSERT(!m_messageHandler->handleMessage(message));
      CPPUNIT_ASSERT(!m_callback_flags[MSG_ID_IDX(MSG_SET_RTC)]);

      strncpy(&message[1], "MAN 15-02-28 18:07:37", MAX_MESSAGE_LENGTH); // Bad day
      CPPUNIT_ASSERT(!m_messageHandler->handleMessage(message));
      CPPUNIT_ASSERT(!m_callback_flags[MSG_ID_IDX(MSG_SET_RTC)]);

      strncpy(&message[1], "SAT 15-04-31 18:07:37", MAX_MESSAGE_LENGTH); // Bad date (> 30 for April)
      CPPUNIT_ASSERT(!m_messageHandler->handleMessage(message));
      CPPUNIT_ASSERT(!m_callback_flags[MSG_ID_IDX(MSG_SET_RTC)]);

      strncpy(&message[1], "SAT 15-01-32 18:07:37", MAX_MESSAGE_LENGTH); // Bad date (> 31 for January)
      CPPUNIT_ASSERT(!m_messageHandler->handleMessage(message));
      CPPUNIT_ASSERT(!m_callback_flags[MSG_ID_IDX(MSG_SET_RTC)]);

      strncpy(&message[1], "SAT 15-01-01 24:07:37", MAX_MESSAGE_LENGTH); // Bad time (> 23 for hour)
      CPPUNIT_ASSERT(!m_messageHandler->handleMessage(message));
      CPPUNIT_ASSERT(!m_callback_flags[MSG_ID_IDX(MSG_SET_RTC)]);
   
      strncpy(&message[1], "SAT 15-01-01 18:60:37", MAX_MESSAGE_LENGTH); // Bad time (> 59 for minute)
      CPPUNIT_ASSERT(!m_messageHandler->handleMessage(message));
      CPPUNIT_ASSERT(!m_callback_flags[MSG_ID_IDX(MSG_SET_RTC)]);

      strncpy(&message[1], "SAT 15-01-01 18:07:60", MAX_MESSAGE_LENGTH); // Bad time (> 59 for second)
      CPPUNIT_ASSERT(!m_messageHandler->handleMessage(message));
      CPPUNIT_ASSERT(!m_callback_flags[MSG_ID_IDX(MSG_SET_RTC)]);

      CPPUNIT_ASSERT_EQUAL(0, callbackSetCount());
   }

   void GetRTCMessageTest()
   {
      m_reply[0] = '\0';
      char message[MAX_MESSAGE_LENGTH] = {MSG_GET_RTC};
      CPPUNIT_ASSERT(m_messageHandler->handleMessage(message));

      CPPUNIT_ASSERT(m_callback_flags[MSG_ID_IDX(MSG_REPLY)]);
      CPPUNIT_ASSERT_EQUAL(1, callbackSetCount());
        
      std::string expected = std::string("  TUE 13-05-21 17:42:23");
      expected[0] = MSG_REPLY;
      expected[1] = MSG_GET_RTC;

      CPPUNIT_ASSERT_EQUAL(expected, m_reply);
      CPPUNIT_ASSERT_EQUAL(23, (int)m_reply.length());
   }

   void SetAlarmMessageTestWithFullDatetime()
   {
      char message[MAX_MESSAGE_LENGTH] = {MSG_SET_ALARM};
      strncpy(&message[1], "01 01Y 10-09 03:45", MAX_MESSAGE_LENGTH);
      CPPUNIT_ASSERT(m_messageHandler->handleMessage(message));


      CPPUNIT_ASSERT_EQUAL(1, m_alarmID);
      CPPUNIT_ASSERT_EQUAL((uint16_t)1, m_alarm.repeat);
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
      CPPUNIT_ASSERT(m_messageHandler->handleMessage(message));


      CPPUNIT_ASSERT_EQUAL(1, m_alarmID);
      CPPUNIT_ASSERT_EQUAL((uint16_t)1, m_alarm.repeat);
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
      CPPUNIT_ASSERT(m_messageHandler->handleMessage(message));


      CPPUNIT_ASSERT_EQUAL(1, m_alarmID);
      CPPUNIT_ASSERT_EQUAL((uint16_t)1, m_alarm.repeat);
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
      CPPUNIT_ASSERT(!m_messageHandler->handleMessage(message));

      strncpy(&message[1], "01 01Y 13", MAX_MESSAGE_LENGTH); // Bad month setting
      CPPUNIT_ASSERT(!m_messageHandler->handleMessage(message));

      strncpy(&message[1], "01 01Y 12-32", MAX_MESSAGE_LENGTH); // Bad date setting
      CPPUNIT_ASSERT(!m_messageHandler->handleMessage(message));

      strncpy(&message[1], "01 01Y 12-31 24:00", MAX_MESSAGE_LENGTH); // Bad hour setting
      CPPUNIT_ASSERT(!m_messageHandler->handleMessage(message));
          
      strncpy(&message[1], "01 01Y 12-31 23:60", MAX_MESSAGE_LENGTH); // Bad minute setting
      CPPUNIT_ASSERT(!m_messageHandler->handleMessage(message));

      strncpy(&message[1], "01 01Y 12/31 2359", MAX_MESSAGE_LENGTH); // Malformed datetime string
      CPPUNIT_ASSERT(!m_messageHandler->handleMessage(message));

      CPPUNIT_ASSERT_EQUAL(0, callbackSetCount());
      CPPUNIT_ASSERT(!m_callback_flags[MSG_ID_IDX(MSG_SET_ALARM)]);
   }

   void SetAlarmMessageTestActionID()
   {
      char message[MAX_MESSAGE_LENGTH] = {MSG_SET_ALARM};

      strncpy(&message[1], "02 01Y", MAX_MESSAGE_LENGTH);
      CPPUNIT_ASSERT(m_messageHandler->handleMessage(message));
      CPPUNIT_ASSERT_EQUAL(2, m_alarmID);
 
      strncpy(&message[1], "03 01Y", MAX_MESSAGE_LENGTH);
      CPPUNIT_ASSERT(m_messageHandler->handleMessage(message));
      CPPUNIT_ASSERT_EQUAL(3, m_alarmID);

      strncpy(&message[1], "16 01Y", MAX_MESSAGE_LENGTH);
      CPPUNIT_ASSERT(m_messageHandler->handleMessage(message));
      CPPUNIT_ASSERT_EQUAL(16, m_alarmID);

      // Invalid action ID
      strncpy(&message[1], "17 01Y", MAX_MESSAGE_LENGTH);
      CPPUNIT_ASSERT(!m_messageHandler->handleMessage(message));
      CPPUNIT_ASSERT_EQUAL(16, m_alarmID);
   }
};

static MessagingTest * s_test_object;

static void setTestObject(void* obj) { s_test_object = (MessagingTest *)obj; }

static bool setRTC_callback(TM*tm)
{
   return s_test_object->SetRTC_callback(tm);
}

static bool setAlarm_callback(int actionID, ALARM * pAlarm)
{
   return s_test_object->SetAlarm_callback(actionID, pAlarm);
}

static void clrAlarm_callback(void)
{
   s_test_object->ClrAlarm_callback();
}

static void setIOType_callback(void)
{
   s_test_object->SetIOType_callback();
}

static void readInput_callback(void)
{
   s_test_object->ReadInput_callback();
}

static void reset_callback(void)
{
   s_test_object->Reset_callback();
}

static void invalid_callback(void)
{
   s_test_object->Invalid_callback();
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