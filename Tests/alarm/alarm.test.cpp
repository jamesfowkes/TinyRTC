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

static const TM s_alarm_datetime = {
   0,  
   05,  
   20, // Hour 
   4, // Date
   JUL, // Month
   96, // Year
   0, //Weekday 
   185, // Day-of-year
   0, // DST
   0, // ignored by util_time
   0 // ignored by util_time
};

class AlarmTest : public CppUnit::TestFixture  {

   CPPUNIT_TEST_SUITE(AlarmTest);
   CPPUNIT_TEST(YearlyAlarmTestDoesNotTriggerWithWrongDate);
   CPPUNIT_TEST(YearlyAlarmTestDoesNotTriggerWithWrongTime);
   CPPUNIT_TEST(YearlyAlarmTestTriggersAtSameDatetime);
   CPPUNIT_TEST(YearlyAlarmTestTriggersAtSameDatetimeWithDifferentYear);
   CPPUNIT_TEST(YearlyAlarmTestWithRepeatTriggersAtCorrectTime);
   CPPUNIT_TEST(YearlyAlarmTestExpiresAfterCorrectDuration);
   CPPUNIT_TEST(YearlyAlarmTestExpiresWhenCancelledEarlyAndIsNotRetriggered);
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

   void YearlyAlarmTestDoesNotTriggerWithWrongDate()
   {
      Alarm alarm = Alarm(INTERVAL_YEAR, &s_alarm_datetime, 1, 60);

      // Alarm should not start on dates that are not JUL04 (check off-by-one errors)
      TM datetime = s_alarm_datetime;
      datetime.tm_mday = 3;
      CPPUNIT_ASSERT(!alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(!alarm.is_triggered());

      datetime.tm_mday = 5;
      CPPUNIT_ASSERT(!alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(!alarm.is_triggered());

      datetime.tm_mday = 4;
      
      datetime.tm_mon = JUN;
      CPPUNIT_ASSERT(!alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(!alarm.is_triggered());

      datetime.tm_mon = AUG;
      CPPUNIT_ASSERT(!alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(!alarm.is_triggered());
   }

   void YearlyAlarmTestDoesNotTriggerWithWrongTime()
   {
      Alarm alarm = Alarm(INTERVAL_YEAR, &s_alarm_datetime, 1, 60);

      // Alarm should not start on times that are not 00:00 (check off-by-one errors)
      TM datetime = s_alarm_datetime;
      datetime.tm_hour = 23;
      datetime.tm_min = 59;
      CPPUNIT_ASSERT(!alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(!alarm.is_triggered());

      datetime.tm_hour = 00;
      datetime.tm_min = 01;
      CPPUNIT_ASSERT(!alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(!alarm.is_triggered());
   }

   void YearlyAlarmTestTriggersAtSameDatetime()
   {
      Alarm alarm = Alarm(INTERVAL_YEAR, &s_alarm_datetime, 1, 60);
     
      CPPUNIT_ASSERT(alarm.set_current_time(&s_alarm_datetime));
      CPPUNIT_ASSERT(alarm.is_triggered());
   }

   void YearlyAlarmTestTriggersAtSameDatetimeWithDifferentYear()
   {
      Alarm alarm = Alarm(INTERVAL_YEAR, &s_alarm_datetime, 1, 60);

      TM datetime = s_alarm_datetime;
      datetime.tm_year = 97;
      CPPUNIT_ASSERT(alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(alarm.is_triggered());
   }

   void YearlyAlarmTestWithRepeatTriggersAtCorrectTime()
   {
      Alarm alarm = Alarm(INTERVAL_YEAR, &s_alarm_datetime, 3, 60);

      TM datetime = s_alarm_datetime;
      datetime.tm_year = 97;
      CPPUNIT_ASSERT(!alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(!alarm.is_triggered());

      datetime.tm_year = 98;
      CPPUNIT_ASSERT(!alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(!alarm.is_triggered());

      datetime.tm_year = 99;
      CPPUNIT_ASSERT(alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(alarm.is_triggered());
   }

   void YearlyAlarmTestExpiresAfterCorrectDuration()
   {
      Alarm alarm = Alarm(INTERVAL_YEAR, &s_alarm_datetime, 1, 60);
      CPPUNIT_ASSERT(alarm.set_current_time(&s_alarm_datetime));

      TM datetime = s_alarm_datetime;

      datetime.tm_hour++;
      CPPUNIT_ASSERT(alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(alarm.is_triggered());
      
      datetime.tm_min++;
      CPPUNIT_ASSERT(!alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(!alarm.is_triggered());
   }

   void YearlyAlarmTestExpiresWhenCancelledEarlyAndIsNotRetriggered()
   {
      Alarm alarm = Alarm(INTERVAL_YEAR, &s_alarm_datetime, 1, 60);
      CPPUNIT_ASSERT(alarm.set_current_time(&s_alarm_datetime));
      CPPUNIT_ASSERT(alarm.is_triggered());
      
      alarm.deactivate();
      CPPUNIT_ASSERT(!alarm.is_triggered());

      TM datetime = s_alarm_datetime;
      datetime.tm_min++;
      CPPUNIT_ASSERT(!alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(!alarm.is_triggered());
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