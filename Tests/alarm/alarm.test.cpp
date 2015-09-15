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

class AlarmTest : public CppUnit::TestFixture  {

   CPPUNIT_TEST_SUITE(AlarmTest);
   CPPUNIT_TEST(YearlyAlarmTest);
   CPPUNIT_TEST_SUITE_END();

public:

   void setUp(void)
   {
      
   }

   void tearDown(void)
   {

   }

private:
   

protected:

   void YearlyAlarmTest()
   {
      ALARM alarm = {
         .datetime = {
            .tm_mon = JUL,
            .tm_mday = 4,
            .tm_hour = 20,
            .tm_min = 00,
            .tm_sec = 00,
         },
         .repeat = 2,
         .repeat_interval = INTERVAL_YEAR,
         .duration = 60,
         .triggered = false,
      };
   }

};

int main()
{
   CppUnit::TextUi::TestRunner runner;
   
   CPPUNIT_TEST_SUITE_REGISTRATION( AlarmTest );

   CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();

   runner.addTest( registry.makeTest() );
   runner.run();

   return 0;
}