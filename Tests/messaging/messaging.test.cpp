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

#include "messaging.h"
#include "ast_node.h"
#include "syntax_parser.h"

static bool setRTC_callback(TM* tm);
static void setTimedAction_callback(void);
static void clrTimedAction_callback(void);
static void setIOType_callback(void);
static void readInput_callback(void);
static void reset_callback(void);
static void invalid_callback(void);
static void reply_callback(char * message);

static void setTestObject(void* obj);

class MessagingTest : public CppUnit::TestFixture  {

   CPPUNIT_TEST_SUITE(MessagingTest);
   CPPUNIT_TEST(ValidSetRTCMessageTest);
   CPPUNIT_TEST(InvalidSetRTCMessageTest);
   CPPUNIT_TEST(GetRTCMessageTest);
   CPPUNIT_TEST_SUITE_END();

public:
   bool SetRTC_callback(TM* tm)
   {
      m_callback_flags[MSG_ID_IDX(MSG_SET_RTC)] = true;
      m_time.tm_year = tm->tm_year;
      m_time.tm_mon = tm->tm_mon;
      m_time.tm_mday = tm->tm_mday;
      m_time.tm_hour = tm->tm_hour;
      m_time.tm_min = tm->tm_min;
      m_time.tm_sec = tm->tm_sec;

      return true;
   }

   void SetTimedAction_callback(void)
   {
      m_callback_flags[MSG_ID_IDX(MSG_SET_TIMED_ACTION)] = true;
   }

   void ClrTimedAction_callback(void)
   {
      m_callback_flags[MSG_ID_IDX(MSG_CLEAR_TIMED_ACTION)] = true;
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

   void Reply_callback(char * message)
   {
      m_reply = std::string(message);
      m_callback_flags[MSG_ID_IDX(MSG_REPLY)] = true;  
   }

   void setUp(void)
   {
      memset(m_callback_flags, false, MSG_MAX_ID);
      m_messageHandler = new MessageHandler(&m_callbacks);

      m_callbacks.setRTCfn = setRTC_callback;
      m_callbacks.getTimedActionfn = setTimedAction_callback;
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
      char message[19] = {MSG_SET_RTC};
      memcpy(&message[1], "15-08-01 18:07:37", 18);
      CPPUNIT_ASSERT(m_messageHandler->handleMessage(message));
      CPPUNIT_ASSERT(m_callback_flags[MSG_ID_IDX(MSG_SET_RTC)]);

      CPPUNIT_ASSERT_EQUAL(15, m_time.tm_year);
      CPPUNIT_ASSERT_EQUAL(8, m_time.tm_mon);
      CPPUNIT_ASSERT_EQUAL(1, m_time.tm_mday);
      CPPUNIT_ASSERT_EQUAL(18, m_time.tm_hour);
      CPPUNIT_ASSERT_EQUAL(07, m_time.tm_min);
      CPPUNIT_ASSERT_EQUAL(37, m_time.tm_sec);

      CPPUNIT_ASSERT_EQUAL(1, callbackSetCount());
   }

   void InvalidSetRTCMessageTest()
   {
      char message[19] = {MSG_SET_RTC};
      memcpy(&message[1], "15-00-01 18:07:37", 18); // Bad month (< 1)
      CPPUNIT_ASSERT(!m_messageHandler->handleMessage(message));
      CPPUNIT_ASSERT(!m_callback_flags[MSG_ID_IDX(MSG_SET_RTC)]);

      memcpy(&message[1], "15-13-01 18:07:37", 18); // Bad month (> 12)
      CPPUNIT_ASSERT(!m_messageHandler->handleMessage(message));
      CPPUNIT_ASSERT(!m_callback_flags[MSG_ID_IDX(MSG_SET_RTC)]);

      memcpy(&message[1], "15-08-00 18:07:37", 18); // Bad date (< 1)
      CPPUNIT_ASSERT(!m_messageHandler->handleMessage(message));
      CPPUNIT_ASSERT(!m_callback_flags[MSG_ID_IDX(MSG_SET_RTC)]);

      memcpy(&message[1], "15-02-29 18:07:37", 18); // Bad date (> 28 for February)
      CPPUNIT_ASSERT(!m_messageHandler->handleMessage(message));
      CPPUNIT_ASSERT(!m_callback_flags[MSG_ID_IDX(MSG_SET_RTC)]);
   
      memcpy(&message[1], "15-04-31 18:07:37", 18); // Bad date (> 30 for April)
      CPPUNIT_ASSERT(!m_messageHandler->handleMessage(message));
      CPPUNIT_ASSERT(!m_callback_flags[MSG_ID_IDX(MSG_SET_RTC)]);

      memcpy(&message[1], "15-01-32 18:07:37", 18); // Bad date (> 31 for January)
      CPPUNIT_ASSERT(!m_messageHandler->handleMessage(message));
      CPPUNIT_ASSERT(!m_callback_flags[MSG_ID_IDX(MSG_SET_RTC)]);

      memcpy(&message[1], "15-01-01 24:07:37", 18); // Bad time (> 23 for hour)
      CPPUNIT_ASSERT(!m_messageHandler->handleMessage(message));
      CPPUNIT_ASSERT(!m_callback_flags[MSG_ID_IDX(MSG_SET_RTC)]);
   
      memcpy(&message[1], "15-01-01 18:60:37", 18); // Bad time (> 59 for minute)
      CPPUNIT_ASSERT(!m_messageHandler->handleMessage(message));
      CPPUNIT_ASSERT(!m_callback_flags[MSG_ID_IDX(MSG_SET_RTC)]);

      memcpy(&message[1], "15-01-01 18:07:60", 18); // Bad time (> 59 for second)
      CPPUNIT_ASSERT(!m_messageHandler->handleMessage(message));
      CPPUNIT_ASSERT(!m_callback_flags[MSG_ID_IDX(MSG_SET_RTC)]);

      CPPUNIT_ASSERT_EQUAL(0, callbackSetCount());
   }

   void GetRTCMessageTest()
   {
      m_reply[0] = '\0';
      char message[] = {MSG_GET_RTC};
      CPPUNIT_ASSERT(m_messageHandler->handleMessage(message));

      CPPUNIT_ASSERT(m_callback_flags[MSG_ID_IDX(MSG_REPLY)]);
      CPPUNIT_ASSERT_EQUAL(1, callbackSetCount());
        
      std::string expected = std::string("  13-04-21 17:42:23");
      expected[0] = MSG_REPLY;
      expected[1] = MSG_GET_RTC;

      CPPUNIT_ASSERT_EQUAL(expected, m_reply);
      CPPUNIT_ASSERT_EQUAL(19, (int)m_reply.length());
   }
};

static MessagingTest * s_test_object;

static void setTestObject(void* obj) { s_test_object = (MessagingTest *)obj; }

static bool setRTC_callback(TM*tm)
{
   s_test_object->SetRTC_callback(tm);
}

static void setTimedAction_callback(void)
{
   s_test_object->SetTimedAction_callback();
}

static void clrTimedAction_callback(void)
{
   s_test_object->ClrTimedAction_callback();
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

static void reply_callback(char * message)
{
   s_test_object->Reply_callback(message);
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