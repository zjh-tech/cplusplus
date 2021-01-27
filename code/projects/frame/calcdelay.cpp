/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-17 17:00:33
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-01-05 14:17:41
 */

#include "calcdelay.h"
#include "engine/inc/common/utils.hpp"
#include "engine/inc/log/env.h"
#include "engine/inc/timer/env.h"
#include <assert.h>

using namespace Framework;

uint64_t CalcDelay::GetNearestDelayEveryMin(int min) {
  if (min < 0 || min >= 60 || (60 % min != 0)) {
    LogErrorA("GetNearestDelayEveryMin min = {}", min);
    return NOVALID_DELAY_MILL_MARCO;
  }

  struct tm tm_cur;
  time_t    cur_time = Utils::GetSecTime();
  Utils::TimeStamp2Tm(cur_time, tm_cur);

  uint64_t remain_time = ONE_HOUR_MILL_MACRO - (tm_cur.tm_min * ONE_MIN_MILL_MACRO + tm_cur.tm_sec * ONE_SEC_MILL_MARCO);
  uint64_t delay       = remain_time % (ONE_MIN_MILL_MACRO * min);
  return delay;
}

uint64_t CalcDelay::GetNearestDelayEveryDay(int hour, int min) {
  if (hour < 0 || hour >= 24 || min < 0 || min >= 60 || 24 % hour != 0) {
    LogErrorA("GetNearestDelayEveryDay hour = {} min = {}", hour, min);
    return NOVALID_DELAY_MILL_MARCO;
  }
  struct tm tm_cur;
  time_t    cur_time = Utils::GetSecTime();
  Utils::TimeStamp2Tm(cur_time, tm_cur);

  time_t delay = 0;

  if ((tm_cur.tm_hour % hour == 0) && tm_cur.tm_min < min) {
    delay = (min - tm_cur.tm_min) * ONE_MIN_MILL_MACRO - ONE_SEC_MILL_MARCO * tm_cur.tm_sec;
  } else {
    //pass triggle time
    uint64_t next_diff_time = hour * ONE_HOUR_MILL_MACRO + min * ONE_MIN_MILL_MACRO;                                                                  //(0-next)
    uint64_t cur_diff_time  = tm_cur.tm_hour % hour * ONE_HOUR_MILL_MACRO + tm_cur.tm_min * ONE_MIN_MILL_MACRO + tm_cur.tm_sec * ONE_SEC_MILL_MARCO;  //(0-cur)
    delay                   = next_diff_time - cur_diff_time;
  }

  return delay;
}

uint64_t CalcDelay::GetNearestDelayEveryWeek(int wday, int hour, int min) {
  if (wday < 1 || wday > 7 || hour < 0 || hour >= 24 || min < 0 || min >= 60) {
    LogErrorA("GetNearestDelayEveryWeek wday = {} hour = {} min = {}", wday, hour, min);
    return NOVALID_DELAY_MILL_MARCO;
  }
  uint64_t cur_time = Utils::GetMillTime();

  uint64_t current_week_triggle_time = CalcDelay::GetCurrentWeekTriggleTime(wday, hour, min, 0);
  if (current_week_triggle_time > cur_time) {
    return (current_week_triggle_time - cur_time);
  } else {
    return (current_week_triggle_time + ONE_DAY_MILL_MACRO * 7 - cur_time);
  }
}

uint64_t CalcDelay::GetNearestDelayEveryMonth(int day, int hour, int min) {
  if (day < 1 || day > 28 || hour < 0 || hour > 24 || min < 0 || min >= 60) {
    LogErrorA("GetNearestDelayEveryMonth day = {} hour = {} min = {}", day, hour, min);
    return NOVALID_DELAY_MILL_MARCO;
  }

  uint64_t cur_time                   = Utils::GetMillTime();
  uint64_t current_month_triggle_time = CalcDelay::GetCurrentMonthTriggleTime(day, hour, min, 0);

  if (current_month_triggle_time > cur_time) {
    return (current_month_triggle_time - cur_time);
  } else {
    //pass current month triggler time
    struct tm tm_cur;
    Utils::TimeStamp2Tm(cur_time, tm_cur);
    int next_month = (tm_cur.tm_mon + 1) % 12;
    if (next_month == 0) {
      next_month = 12;
    }
    tuple<bool, uint64_t> attr                  = Utils::GetTotalDayCount(next_month, tm_cur.tm_year);
    uint64_t              tatal_day_count       = get<1>(attr);
    uint64_t              next_month_total_time = current_month_triggle_time + ONE_MONTH_MILL_MACRO(tatal_day_count);
    uint64_t              delay                 = next_month_total_time - cur_time;
    return delay;
  }
}

uint64_t CalcDelay::GetCurrentWeekTriggleTime(int wday, int hour, int min, int sec) {
  time_t    cur_time = Utils::GetSecTime();
  struct tm tm_cur   = {0};
  Utils::TimeStamp2Tm(cur_time, tm_cur);
  tm_cur.tm_hour = hour;
  tm_cur.tm_min  = min;
  tm_cur.tm_sec  = sec;

  time_t ret_sec_time = mktime(&tm_cur);
  if (tm_cur.tm_wday > wday) {
    int temp_sec = (tm_cur.tm_wday - wday) * ONE_DAY_MACRO;
    ret_sec_time -= temp_sec;
  } else if (tm_cur.tm_wday < wday) {
    int temp_sec = (wday - tm_cur.tm_wday) * ONE_DAY_MACRO;
    ret_sec_time += temp_sec;
  }

  uint64_t ret_mill_sec_time = ret_sec_time * 1000;
  return ret_mill_sec_time;
}

uint64_t CalcDelay::GetCurrentMonthTriggleTime(int day, int hour, int min, int sec) {
  time_t    cur_time = Utils::GetSecTime();
  struct tm tm_cur   = {0};
  Utils::TimeStamp2Tm(cur_time, tm_cur);

  tm_cur.tm_mday = day;
  tm_cur.tm_hour = hour;
  tm_cur.tm_min  = min;
  tm_cur.tm_sec  = sec;

  time_t   ret_sec_time      = mktime(&tm_cur);
  uint64_t ret_mill_sec_time = ret_sec_time * 1000;
  return ret_mill_sec_time;
}