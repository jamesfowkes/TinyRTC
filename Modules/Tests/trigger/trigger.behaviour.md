# Trigger Behaviour

A trigger shall:

* register 
* be triggered at specific times. The trigger time:
  * can be each year, each month, each week, each day or each hour
  * can be set with precision to the minute:
    * an hourly alarm can be set to any minute within that hour (defaulting to 00)
    * a daily alarm can be set to any time within that day (defaulting to 00:00)
    * a weekly alarm can be set to any day within that week and time within that day (defaulting to Monday at 00:00)
    * a monthly alarm can be set to any date within that month and time within that day (defaulting to the 1st at 00:00)
    * a yearly alarm can be set to any month within that year, date within that month and time within that day (defaulting to 1st January at 00:00)
* be triggered when a number of trigger times between 1 and 99 have passed
  * for example, setting N to 2 for a weekly interval would result in a fortnightly alarm trigger
* if triggered, change state to untriggered after a duration of N minutes have passed
* if triggered, change state to untriggered by an external event
