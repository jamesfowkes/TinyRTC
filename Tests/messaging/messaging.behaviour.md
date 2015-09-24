# Messaging Behaviour

The messaging handler shall:

* Allow setting the onboard RTC:
  * accept messages of the form 'A DDD YY-MM-DD HH:MM:SS' where:
  * A is the unique message ID indicating a "Set RTC" message
  * DDD is the day of week (e.g. MON, TUE, WED etc)
  * YY-MM-DD HH:MM:SS are the date and time
  * return a reply message of:
   * '>A OK' if successful
   * '>A FAIL' if badly formed message or setting failed

* Allow reading the onboard RTC:
  * accept messages of the form 'B' where B is the unique message ID indicating a "Get RTC" message
  * return a string of the form '>B DDD YY-MM-DD HH:MM:SS', where the datetime string is the application RTC time

* Allow setting of alarms:
  * accept messages of the form 'C AA IIY [MM[-DD[ HH[:MM]]]] [DNNNN]' where:
    * C is the unique message ID indicating a "Set alarm" message
    * AA is an alarm ID between 00 and 99 or an application defined maximum
    * II is an interval between 00 and 99
    * Y indicates that this is a yearly interval
    * DNNNN is an optional alarm duration in minutes
    * The remaining characters are optional datetime specifiers
    * For example the message 'C 02 01Y 08-02 12 D0030' sets alarm 2 every year on 2nd August at 12:00, with a duration of 30 minutes

  * accept messages of the form 'C AA IIM [-DD[ HH[:MM]]] [DNNNN]' where:
    * C, AA, II and DNNNN are as described above
    * M indicates that this is a monthly interval
    * The remaining characters are optional datetime specifiers
    * For example the message 'C 01 02M 20 10:30' sets alarm 1 every 2nd month on the 20th of that month at 10:30. The duration shall be set to the alarm default.

  * accept messages of the form 'C AA IIW [DDD] [HH[:MM]] [DNNNN]' where:
    * C, AA, II and DNNNN are as described above
    * W indicates that this is a weekly interval
    * The remaining characters are optional datetime specifiers
    * For example the message 'C 03 02W TUE 05:00 D0240' sets alarm 3 every second Tuesday at 05:00, with a duration of 240 minutes (4 hours)

  * accept messages of the form 'C AA IID [HH[:MM]] [DNNNN]' where:
    * C, AA, II and DNNNN are as described above
    * D indicates that this is a monthly interval
    * The remaining characters are optional datetime specifiers
    * For example the message 'C 01 01D' sets alarm 1 every day. The duration shall be set to the alarm default.

  * for all the above messages, create and register an alarm object with the application

  * for all the above messages, return a reply message of:
   * '>C OK' if successful
   * '>C FAIL' if badly formed message or setting failed

* Allow clearing of alarms:
  * accept messages of the torm 'D AA' where:
    * E is the unique message ID indicating a "Clear alarm" message
    * AA is an alarm ID between 00 and 99 or an application defined maximum
  
  * request that the appropriate alarm be cleared

  * return a reply message of:
   * '>D OK' if successful
   * '>D FAIL' if badly formed message or clearing failed

* Allow setting of triggers for outputs
  * accept messages of the form 'E A expr' where:
    * E is the unique message ID indicating a "Set trigger" message
    * A is an output ID between 0 and 9
    * expr is a logical expression that describes the conditions under which the output should activate. See syntax parser documents for expression details.

  * set the output trigger expression

  * return a reply message of:
   * '>E OK' if output trigger set successful
   * '>E FAIL' if badly formed message or setting failed

* Allow clearing of triggers for outputs
  * accept messages of the form 'F A' where:
    * F is the unique message ID indicating a "Set trigger" message
    * A is an output ID between 0 and 9

  * clear the output trigger

  * return a reply message of:
   * '>F OK' if output trigger clear successful
   * '>F FAIL' if badly formed message or clearing failed

* Allow setting of IO types (input or output)
  * accept messages of the form 'G A IN' or 'G A OUT' where:
    * G is the unique message ID indicating a "Set IO type" message
    * A is an output ID between 0 and 9
    * IN or OUT specify the IO type

    * configure the input

    * return a reply message of:
      * '>G OK' if output trigger clear successful
      * '>G FAIL' if badly formed message or clearing failed

* Allow reading of input states
  * accept messages of the form 'H A' where:
    * H is the unique message ID indicating a "Read input" message
    * A is an output ID between 0 and 9

    * return a reply message of:
      * '>H A 1' where A is the output ID, if the input is on
      * '>H A 0' where A is the output ID, if the input is off

* Allow reset of the application
  * accept messages of the form 'I', where:
    * I is the unique message ID indicating a "reset" message

  * return a reply message of '>RESET' and reset the application