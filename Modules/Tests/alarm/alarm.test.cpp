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
   SUN, //Weekday 
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
   CPPUNIT_TEST(YearlyAlarmTestTriggersWhenYearChanged);
   CPPUNIT_TEST(YearlyAlarmTestWithRepeatTriggersAtCorrectTime);
   CPPUNIT_TEST(YearlyAlarmTestExpiresAfterCorrectDuration);
   CPPUNIT_TEST(YearlyAlarmTestExpiresWhenCancelledEarlyAndIsNotRetriggered);
   
   CPPUNIT_TEST(MonthlyAlarmTestDoesNotTriggerWithWrongDate);
   CPPUNIT_TEST(MonthlyAlarmTestDoesNotTriggerWithWrongTime);
   CPPUNIT_TEST(MonthlyAlarmTestTriggersAtSameDatetime);
   CPPUNIT_TEST(MonthlyAlarmTestWhenYearAndMonthChanged);
   CPPUNIT_TEST(MonthlyAlarmTestWithRepeatTriggersAtCorrectTime);
   CPPUNIT_TEST(MonthlyAlarmTestExpiresAfterCorrectDuration);
   CPPUNIT_TEST(MonthlyAlarmTestExpiresWhenCancelledEarlyAndIsNotRetriggered);

   CPPUNIT_TEST(WeeklyAlarmTestDoesNotTriggerWithWrongDate);
   CPPUNIT_TEST(WeeklyAlarmTestDoesNotTriggerWithWrongTime);
   CPPUNIT_TEST(WeeklyAlarmTestTriggersAtSameDatetime);
   CPPUNIT_TEST(WeeklyAlarmTestTriggersWhenMonthYearAndDateChanged);
   CPPUNIT_TEST(WeeklyAlarmTestWithRepeatTriggersAtCorrectTime);
   CPPUNIT_TEST(WeeklyAlarmTestExpiresAfterCorrectDuration);
   CPPUNIT_TEST(WeeklyAlarmTestExpiresWhenCancelledEarlyAndIsNotRetriggered);

   CPPUNIT_TEST(DailyAlarmTestDoesNotTriggerWithWrongTime);
   CPPUNIT_TEST(DailyAlarmTestTriggersAtSameDatetime);
   CPPUNIT_TEST(DailyAlarmTestTriggersWhenYearMonthDateWeekdayChanged);
   CPPUNIT_TEST(DailyAlarmTestWithRepeatTriggersAtCorrectTime);
   CPPUNIT_TEST(DailyAlarmTestExpiresAfterCorrectDuration);
   CPPUNIT_TEST(DailyAlarmTestExpiresWhenCancelledEarlyAndIsNotRetriggered);

   CPPUNIT_TEST(HourlyAlarmTestDoesNotTriggerWithWrongTime);
   CPPUNIT_TEST(HourlyAlarmTestTriggersAtSameDatetime);
   CPPUNIT_TEST(HourlyAlarmTestTriggersWhenYearMonthDateWeekdayHourChanged);
   CPPUNIT_TEST(HourlyAlarmTestWithRepeatTriggersAtCorrectTime);
   CPPUNIT_TEST(HourlyAlarmTestExpiresAfterCorrectDuration);
   CPPUNIT_TEST(HourlyAlarmTestExpiresWhenCancelledEarlyAndIsNotRetriggered);

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

   void YearlyAlarmTestTriggersWhenYearChanged()
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
      datetime.tm_year++;
      CPPUNIT_ASSERT(!alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(!alarm.is_triggered());

      datetime.tm_year++;
      CPPUNIT_ASSERT(!alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(!alarm.is_triggered());

      datetime.tm_year++;
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

   void MonthlyAlarmTestDoesNotTriggerWithWrongDate()
   {
      Alarm alarm = Alarm(INTERVAL_MONTH, &s_alarm_datetime, 1, 60);

      // Alarm should not start on dates that are not 04 (check off-by-one errors)
      TM datetime = s_alarm_datetime;
      datetime.tm_mday = 3;
      CPPUNIT_ASSERT(!alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(!alarm.is_triggered());

      datetime.tm_mday = 5;
      CPPUNIT_ASSERT(!alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(!alarm.is_triggered());
   }

   void MonthlyAlarmTestDoesNotTriggerWithWrongTime()
   {
      Alarm alarm = Alarm(INTERVAL_MONTH, &s_alarm_datetime, 1, 60);

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

   void MonthlyAlarmTestTriggersAtSameDatetime()
   {
      Alarm alarm = Alarm(INTERVAL_MONTH, &s_alarm_datetime, 1, 60);
     
      CPPUNIT_ASSERT(alarm.set_current_time(&s_alarm_datetime));
      CPPUNIT_ASSERT(alarm.is_triggered());
   }

   void MonthlyAlarmTestWhenYearAndMonthChanged()
   {
      Alarm alarm = Alarm(INTERVAL_MONTH, &s_alarm_datetime, 1, 60);

      TM datetime = s_alarm_datetime;
      datetime.tm_year = 97;
      CPPUNIT_ASSERT(alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(alarm.is_triggered());

      alarm = Alarm(INTERVAL_MONTH, &s_alarm_datetime, 1, 60);
      datetime.tm_mon = APR;
      CPPUNIT_ASSERT(alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(alarm.is_triggered());
   }

   void MonthlyAlarmTestWithRepeatTriggersAtCorrectTime()
   {
      Alarm alarm = Alarm(INTERVAL_MONTH, &s_alarm_datetime, 3, 60);

      TM datetime = s_alarm_datetime;
      datetime.tm_mon = AUG;
      CPPUNIT_ASSERT(!alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(!alarm.is_triggered());

      datetime.tm_mon = SEP;
      CPPUNIT_ASSERT(!alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(!alarm.is_triggered());

      datetime.tm_mon = OCT;
      CPPUNIT_ASSERT(alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(alarm.is_triggered());
   }

   void MonthlyAlarmTestExpiresAfterCorrectDuration()
   {
      Alarm alarm = Alarm(INTERVAL_MONTH, &s_alarm_datetime, 1, 60);
      CPPUNIT_ASSERT(alarm.set_current_time(&s_alarm_datetime));

      TM datetime = s_alarm_datetime;

      datetime.tm_hour++;
      CPPUNIT_ASSERT(alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(alarm.is_triggered());
      
      datetime.tm_min++;
      CPPUNIT_ASSERT(!alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(!alarm.is_triggered());
   }

   void MonthlyAlarmTestExpiresWhenCancelledEarlyAndIsNotRetriggered()
   {
      Alarm alarm = Alarm(INTERVAL_MONTH, &s_alarm_datetime, 1, 60);
      CPPUNIT_ASSERT(alarm.set_current_time(&s_alarm_datetime));
      CPPUNIT_ASSERT(alarm.is_triggered());
      
      alarm.deactivate();
      CPPUNIT_ASSERT(!alarm.is_triggered());

      TM datetime = s_alarm_datetime;
      datetime.tm_min++;
      CPPUNIT_ASSERT(!alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(!alarm.is_triggered());
   }

   void WeeklyAlarmTestDoesNotTriggerWithWrongDate()
   {
      Alarm alarm = Alarm(INTERVAL_WEEK, &s_alarm_datetime, 1, 60);

      // Alarm should not start on days that are not SUN (check off-by-one errors)
      TM datetime = s_alarm_datetime;
      datetime.tm_wday = SAT;
      CPPUNIT_ASSERT(!alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(!alarm.is_triggered());

      datetime.tm_mday = MON;
      CPPUNIT_ASSERT(!alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(!alarm.is_triggered());
   }

   void WeeklyAlarmTestDoesNotTriggerWithWrongTime()
   {
      Alarm alarm = Alarm(INTERVAL_WEEK, &s_alarm_datetime, 1, 60);

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

   void WeeklyAlarmTestTriggersAtSameDatetime()
   {
      Alarm alarm = Alarm(INTERVAL_WEEK, &s_alarm_datetime, 1, 60);
     
      CPPUNIT_ASSERT(alarm.set_current_time(&s_alarm_datetime));
      CPPUNIT_ASSERT(alarm.is_triggered());
   }

   void WeeklyAlarmTestTriggersWhenMonthYearAndDateChanged()
   {
      Alarm alarm = Alarm(INTERVAL_WEEK, &s_alarm_datetime, 1, 60);

      TM datetime = s_alarm_datetime;
      datetime.tm_year = 97;
      CPPUNIT_ASSERT(alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(alarm.is_triggered());

      alarm = Alarm(INTERVAL_WEEK, &s_alarm_datetime, 1, 60);
      datetime.tm_mon = APR;
      CPPUNIT_ASSERT(alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(alarm.is_triggered());

      alarm = Alarm(INTERVAL_WEEK, &s_alarm_datetime, 1, 60);
      datetime.tm_mday = 05;
      CPPUNIT_ASSERT(alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(alarm.is_triggered());
   }

   void WeeklyAlarmTestWithRepeatTriggersAtCorrectTime()
   {
      Alarm alarm = Alarm(INTERVAL_WEEK, &s_alarm_datetime, 3, 60);

      TM datetime = s_alarm_datetime;
      datetime.tm_mday += 7;
      CPPUNIT_ASSERT(!alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(!alarm.is_triggered());

      datetime.tm_mday += 7;
      CPPUNIT_ASSERT(!alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(!alarm.is_triggered());

      datetime.tm_mday += 7;
      CPPUNIT_ASSERT(alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(alarm.is_triggered());
   }

   void WeeklyAlarmTestExpiresAfterCorrectDuration()
   {
      Alarm alarm = Alarm(INTERVAL_WEEK, &s_alarm_datetime, 1, 60);
      CPPUNIT_ASSERT(alarm.set_current_time(&s_alarm_datetime));

      TM datetime = s_alarm_datetime;

      datetime.tm_hour++;
      CPPUNIT_ASSERT(alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(alarm.is_triggered());
      
      datetime.tm_min++;
      CPPUNIT_ASSERT(!alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(!alarm.is_triggered());
   }

   void WeeklyAlarmTestExpiresWhenCancelledEarlyAndIsNotRetriggered()
   {
      Alarm alarm = Alarm(INTERVAL_WEEK, &s_alarm_datetime, 1, 60);
      CPPUNIT_ASSERT(alarm.set_current_time(&s_alarm_datetime));
      CPPUNIT_ASSERT(alarm.is_triggered());
      
      alarm.deactivate();
      CPPUNIT_ASSERT(!alarm.is_triggered());

      TM datetime = s_alarm_datetime;
      datetime.tm_min++;
      CPPUNIT_ASSERT(!alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(!alarm.is_triggered());
   }

   void DailyAlarmTestDoesNotTriggerWithWrongTime()
   {
      Alarm alarm = Alarm(INTERVAL_DAY, &s_alarm_datetime, 1, 60);

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

   void DailyAlarmTestTriggersAtSameDatetime()
   {
      Alarm alarm = Alarm(INTERVAL_DAY, &s_alarm_datetime, 1, 60);
     
      CPPUNIT_ASSERT(alarm.set_current_time(&s_alarm_datetime));
      CPPUNIT_ASSERT(alarm.is_triggered());
   }

   void DailyAlarmTestTriggersWhenYearMonthDateWeekdayChanged()
   {
      Alarm alarm = Alarm(INTERVAL_DAY, &s_alarm_datetime, 1, 60);

      TM datetime = s_alarm_datetime;
      datetime.tm_year = 97;
      CPPUNIT_ASSERT(alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(alarm.is_triggered());

      alarm = Alarm(INTERVAL_DAY, &s_alarm_datetime, 1, 60);
      datetime.tm_mon = APR;
      CPPUNIT_ASSERT(alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(alarm.is_triggered());

      alarm = Alarm(INTERVAL_DAY, &s_alarm_datetime, 1, 60);
      datetime.tm_mday = 05;
      CPPUNIT_ASSERT(alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(alarm.is_triggered());

      alarm = Alarm(INTERVAL_DAY, &s_alarm_datetime, 1, 60);
      datetime.tm_wday = MON;
      CPPUNIT_ASSERT(alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(alarm.is_triggered());
   }

   void DailyAlarmTestWithRepeatTriggersAtCorrectTime()
   {
      Alarm alarm = Alarm(INTERVAL_DAY, &s_alarm_datetime, 3, 60);

      TM datetime = s_alarm_datetime;
      datetime.tm_mday++;
      CPPUNIT_ASSERT(!alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(!alarm.is_triggered());

      datetime.tm_mday++;
      CPPUNIT_ASSERT(!alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(!alarm.is_triggered());

      datetime.tm_mday++;
      CPPUNIT_ASSERT(alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(alarm.is_triggered());
   }

   void DailyAlarmTestExpiresAfterCorrectDuration()
   {
      Alarm alarm = Alarm(INTERVAL_DAY, &s_alarm_datetime, 1, 60);
      CPPUNIT_ASSERT(alarm.set_current_time(&s_alarm_datetime));

      TM datetime = s_alarm_datetime;

      datetime.tm_hour++;
      CPPUNIT_ASSERT(alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(alarm.is_triggered());
      
      datetime.tm_min++;
      CPPUNIT_ASSERT(!alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(!alarm.is_triggered());
   }

   void DailyAlarmTestExpiresWhenCancelledEarlyAndIsNotRetriggered()
   {
      Alarm alarm = Alarm(INTERVAL_DAY, &s_alarm_datetime, 1, 60);
      CPPUNIT_ASSERT(alarm.set_current_time(&s_alarm_datetime));
      CPPUNIT_ASSERT(alarm.is_triggered());
      
      alarm.deactivate();
      CPPUNIT_ASSERT(!alarm.is_triggered());

      TM datetime = s_alarm_datetime;
      datetime.tm_min++;
      CPPUNIT_ASSERT(!alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(!alarm.is_triggered());
   }

   void HourlyAlarmTestDoesNotTriggerWithWrongTime()
   {
      Alarm alarm = Alarm(INTERVAL_HOUR, &s_alarm_datetime, 1, 60);

      // Alarm should not start on times that are not ??:00 (check off-by-one errors)
      TM datetime = s_alarm_datetime;
      datetime.tm_min = 59;
      CPPUNIT_ASSERT(!alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(!alarm.is_triggered());

      datetime.tm_min = 01;
      CPPUNIT_ASSERT(!alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(!alarm.is_triggered());
   }

   void HourlyAlarmTestTriggersAtSameDatetime()
   {
      Alarm alarm = Alarm(INTERVAL_HOUR, &s_alarm_datetime, 1, 60);
     
      CPPUNIT_ASSERT(alarm.set_current_time(&s_alarm_datetime));
      CPPUNIT_ASSERT(alarm.is_triggered());
   }

   void HourlyAlarmTestTriggersWhenYearMonthDateWeekdayHourChanged()
   {
      Alarm alarm = Alarm(INTERVAL_HOUR, &s_alarm_datetime, 1, 60);

      TM datetime = s_alarm_datetime;
      datetime.tm_year = 97;
      CPPUNIT_ASSERT(alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(alarm.is_triggered());

      alarm = Alarm(INTERVAL_HOUR, &s_alarm_datetime, 1, 60);
      datetime.tm_mon = APR;
      CPPUNIT_ASSERT(alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(alarm.is_triggered());

      alarm = Alarm(INTERVAL_HOUR, &s_alarm_datetime, 1, 60);
      datetime.tm_mday = 05;
      CPPUNIT_ASSERT(alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(alarm.is_triggered());

      alarm = Alarm(INTERVAL_HOUR, &s_alarm_datetime, 1, 60);
      datetime.tm_wday = MON;
      CPPUNIT_ASSERT(alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(alarm.is_triggered());

      alarm = Alarm(INTERVAL_HOUR, &s_alarm_datetime, 1, 60);
      datetime.tm_hour = 01;
      CPPUNIT_ASSERT(alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(alarm.is_triggered());
   }

   void HourlyAlarmTestWithRepeatTriggersAtCorrectTime()
   {
      Alarm alarm = Alarm(INTERVAL_HOUR, &s_alarm_datetime, 3, 60);

      TM datetime = s_alarm_datetime;
      datetime.tm_hour++;
      CPPUNIT_ASSERT(!alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(!alarm.is_triggered());

      datetime.tm_hour++;
      CPPUNIT_ASSERT(!alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(!alarm.is_triggered());

      datetime.tm_hour++;
      CPPUNIT_ASSERT(alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(alarm.is_triggered());
   }

   void HourlyAlarmTestExpiresAfterCorrectDuration()
   {
      Alarm alarm = Alarm(INTERVAL_HOUR, &s_alarm_datetime, 1, 30);
      CPPUNIT_ASSERT(alarm.set_current_time(&s_alarm_datetime));

      TM datetime = s_alarm_datetime;

      datetime.tm_min += 30;
      CPPUNIT_ASSERT(alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(alarm.is_triggered());
      
      datetime.tm_min++;
      CPPUNIT_ASSERT(!alarm.set_current_time(&datetime));
      CPPUNIT_ASSERT(!alarm.is_triggered());
   }

   void HourlyAlarmTestExpiresWhenCancelledEarlyAndIsNotRetriggered()
   {
      Alarm alarm = Alarm(INTERVAL_HOUR, &s_alarm_datetime, 1, 60);
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