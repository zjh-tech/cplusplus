/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-17 16:58:25
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-01-04 14:55:39
 */

#pragma once

#include <cinttypes>
#include <time.h>

class CalcDelay {
public:
  //everyhour  min   5 ==> triggle time (1.5) (2.5) (3.5)...
  //condition  60 % min == 0
  static uint64_t GetNearestDelayEveryMin(int min);

  //everyday  hour = 0 min = 0 ==> triggle time(1.0.0) (2.0.0) (3.0.0)...
  //hour(1-23) min(0-59)
  //condition  24 % hour == 0
  static uint64_t GetNearestDelayEveryDay(int hour, int min);

  //everyweek  hour = 0 min = 0
  //wday(1-7)hour(0-23)min(0-59)
  static uint64_t GetNearestDelayEveryWeek(int wday, int hour, int min);

  //everymonth day = 1 hour = 0 min = 0 ==> triggle time(1.0.0.0) (2.0.0.0)
  //day(1-28)hour(0-23)min(0-59)
  static uint64_t GetNearestDelayEveryMonth(int day, int hour, int min);

public:
  static uint64_t GetCurrentWeekTriggleTime(int wday, int hour, int min, int sec);
  static uint64_t GetCurrentMonthTriggleTime(int day, int hour, int min, int sec);
};
