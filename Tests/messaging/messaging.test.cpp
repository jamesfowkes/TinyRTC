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

#include "messaging.h"
#include "ast_node.h"
#include "syntax_parser.h"

static bool setRTC_callback(uint8_t yy, uint8_t mmm, uint8_t dd, uint8_t hh, uint8_t mm, uint8_t ss);
static void getRTC_callback(void);
static void setTimedAction_callback(void);
static void clrTimedAction_callback(void);
static void setIOType_callback(void);
static void readInput_callback(void);
static void reset_callback(void);
static void invalid_callback(void);
static void setTestObject(void* obj);

class MessagingTest : public CppUnit::TestFixture  {

   CPPUNIT_TEST_SUITE(MessagingTest);
   CPPUNIT_TEST(ValidSetRTCMessageTest);
   CPPUNIT_TEST(InvalidSetRTCMessageTest);
   CPPUNIT_TEST_SUITE_END();

public:
   bool SetRTC_callback(uint8_t yy, uint8_t mmm, uint8_t dd, uint8_t hh, uint8_t mm, uint8_t ss)
   {
      m_callback_flags[MSG_SET_RTC] = true;
      m_yy = yy;
      m_mmm = mmm;
      m_dd = dd;
      m_hh = hh;
      m_mm = mm;
      m_ss = ss;

      return true;
   }

   void GetRTC_callback(void)
   {
      m_callback_flags[MSG_GET_RTC] = true;
   }

   void SetTimedAction_callback(void)
   {
      m_callback_flags[MSG_SET_TIMED_ACTION] = true;
   }

   void ClrTimedAction_callback(void)
   {
      m_callback_flags[MSG_CLEAR_TIMED_ACTION] = true;
   }

   void SetIOType_callback(void)
   {
      m_callback_flags[MSG_SET_IO_TYPE] = true;  
   }

   void ReadInput_callback(void)
   {
      m_callback_flags[MSG_READ_INPUT] = true;  
   }

   void Reset_callback(void)
   {
      m_callback_flags[MSG_RESET] = true;  
   }

   void Invalid_callback(void)
   {
      m_callback_flags[MSG_INVALID] = true;  
   }

   void setUp(void)
   {
      memset(m_callback_flags, false, MSG_MAX_ID);
      m_messageHandler = new MessageHandler(&m_callbacks);

      m_callbacks.setRTCfn = setRTC_callback;
      m_callbacks.getRTCfn = getRTC_callback;
      m_callbacks.getTimedActionfn = setTimedAction_callback;
      m_callbacks.setIOTypefn = setIOType_callback;
      m_callbacks.readInputfn = readInput_callback;
      m_callbacks.resetfn = reset_callback;
      m_callbacks.invalidfn = invalid_callback;

      setTestObject(this);
   }

   void tearDown(void)
   {
      free(m_messageHandler);
   }

private:
   
   bool m_callback_flags[MSG_MAX_ID];

   uint8_t m_yy, m_mmm, m_dd, m_hh, m_mm, m_ss;

   MessageHandler * m_messageHandler;

   MSG_HANDLER_FUNCTIONS m_callbacks;
   
protected:

   void ValidSetRTCMessageTest()
   {
      char message[] = {
         MSG_SET_RTC,
         '1','5',
         '-',
         '0','8',
         '-',
         '0','1',
         ' ',
         '1','8',
         ':',
         '0','7',
         ':',
         '3','4',
         '\0'
      };
      m_messageHandler->handleMessage(message);
      CPPUNIT_ASSERT(m_callback_flags[MSG_SET_RTC]);
   }
};

static MessagingTest * s_test_object;

static void setTestObject(void* obj) { s_test_object = (MessagingTest *)obj; }

static bool setRTC_callback(uint8_t yy, uint8_t mmm, uint8_t dd, uint8_t hh, uint8_t mm, uint8_t ss)
{
   s_test_object->SetRTC_callback(yy, mmm, dd, hh, mm, ss);
}

static void getRTC_callback(void)
{
   s_test_object->GetRTC_callback();
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

int main()
{
   CppUnit::TextUi::TestRunner runner;
   
   CPPUNIT_TEST_SUITE_REGISTRATION( MessagingTest );

   CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();

   runner.addTest( registry.makeTest() );
   runner.run();

   return 0;
}