/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-15 20:00:18
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-04-24 17:57:58
 */

#include "engine/inc/common/utils.hpp"
#include "engine/inc/log/logger.h"
#include "engine/inc/object_pool/env.h"
#include "engine/inc/timer/env.h"

enum eTimerID {
  TEST_ID = 1,
};

struct STestData {
  uint32_t a        = 0;
  uint64_t b        = 0;
  int32_t  loop_num = 3;
};

enum eGlobalTimerID {
  EVERY_SOME_MIN_CLOCK   = 1,  //everyhour min
  EVERY_SOME_HOUR_CLOCK  = 2,  //everyday hour  min
  EVERY_SOME_WDAY_CLOCK  = 3,  //everyweek hour min
  EVERY_SOME_MONTH_CLOCK = 4,  //everymonth day min
};

static void testdata_release(void* data) {
  STestData* pTest = (STestData*)data;
  delete pTest;
  LogInfoA("[timer] testdata_release");
}

void global_timer_min() {
  LogInfoA("global_timer_min exec");
}

void global_timer_hour() {
  LogInfoA("global_timer_hour exec");
}

void global_timer_wday() {
  LogInfoA("global_timer_wday exec");
}

void global_timer_month() {
  LogInfoA("global_timer_month exec");
}

void test_global_timer() {
  int      min           = 5;
  uint64_t min_timestamp = CalcDelay::GetNearestDelayEveryMin(min);
  GTimerRegister->add_once_timer(
    eGlobalTimerID::EVERY_SOME_MIN_CLOCK, min_timestamp, [min](void*) {
      global_timer_min();

      GTimerRegister->add_repeat_timer(eGlobalTimerID::EVERY_SOME_MIN_CLOCK, ONE_MIN_MILL_MACRO * min, [](void*) {
        global_timer_min();
      });
    });

  int      hour           = 1;
  uint64_t hour_timestamp = CalcDelay::GetNearestDelayEveryDay(hour, 10);
  GTimerRegister->add_once_timer(
    eGlobalTimerID::EVERY_SOME_HOUR_CLOCK, hour_timestamp, [hour](void*) {
      global_timer_hour();

      GTimerRegister->add_repeat_timer(eGlobalTimerID::EVERY_SOME_HOUR_CLOCK, ONE_HOUR_MILL_MACRO * hour, [](void*) {
        global_timer_hour();
      });
    });

  int      wday           = 3;
  uint64_t wday_timestamp = CalcDelay::GetNearestDelayEveryWeek(wday, 10, 10);
  GTimerRegister->add_once_timer(
    eGlobalTimerID::EVERY_SOME_WDAY_CLOCK, wday_timestamp, [wday](void*) {
      global_timer_wday();

      GTimerRegister->add_repeat_timer(eGlobalTimerID::EVERY_SOME_WDAY_CLOCK, ONE_DAY_MILL_MACRO * 7, [](void*) {
        global_timer_wday();
      });
    });

  uint64_t month_timestamp = CalcDelay::GetNearestDelayEveryMonth(23, 10, 10);
  GTimerRegister->add_once_timer(
    eGlobalTimerID::EVERY_SOME_MONTH_CLOCK, month_timestamp, [](void*) {
      global_timer_month();

      struct tm tm_cur = {0};
      Utils::TimeStamp2Tm(Utils::GetSecTime(), tm_cur);
      uint64_t tatal_day_count = Utils::GetTotalDayCount(tm_cur.tm_mon, tm_cur.tm_year);
      GTimerRegister->add_once_timer(eGlobalTimerID::EVERY_SOME_MONTH_CLOCK, ONE_MONTH_MILL_MACRO(tatal_day_count), [](void*) {
        global_timer_month();
      });
    });
}

class CTest {
public:
  virtual ~CTest() {
    m_timer_register = nullptr;
  }

  void test_once_timer(STestData* attach) {
    int64_t delay = 1000 * 30;

    m_timer_register->add_once_timer(
      eTimerID::TEST_ID, delay, [this](void* data) {
        STestData* pTest = (STestData*)data;
        --pTest->loop_num;
        LogInfoA("[timer] test timer exec a = {} b = {}", pTest->a, pTest->b);
        if (pTest->loop_num > 0) {
          test_once_timer(pTest);
        }
      },
      attach,
      testdata_release);
  }

  void test_repeat_timer() {
    int64_t delay = 1000 * 30;

    STestData* attach1 = new STestData();
    attach1->a         = 1;
    attach1->b         = 2;

    m_timer_register->add_repeat_timer(
      eTimerID::TEST_ID, delay, [](void* data) {
        STestData* pTest = (STestData*)data;
        LogInfoA("[timer] test timer exec a = {} b = {}", pTest->a, pTest->b);

        //std::this_thread::sleep_for(std::chrono::seconds(2));  //need modify delay
      },
      attach1,
      testdata_release);
  }

private:
  std::shared_ptr<TimerRegister> m_timer_register = std::make_shared<TimerRegister>();
};

//1  add ==> triggle before delete and add new
//2  add ==> add new
//3  add_once_timer  add_repeat_timer
//4  add_once_timer in  add_once_timer ==> repeat attach data not share
//5  callback cost too long time ==> modify delay

int main(void) {
  LOG_INIT("timer_example", true, "debug", true);
  TIMER_INIT();

  GObjectPoolMgr = std::make_shared<ObjectPoolMgr>(false);

  CTest      test;
  STestData* attach = new STestData();
  attach->a         = 1;
  attach->b         = 2;
  test.test_once_timer(attach);
  //test.test_repeat_timer();

  //test_global_timer();

  // uint64_t time1 = Utils::GetMillTime();
  // bool triggler = false;
  while (true) {
    TimeWheelMgr::Instance()->run();

    // uint64_t time2 = Utils::GetMillTime();
    // if (((time2 - time1) >= 10 * 1000) && !triggler) {
    //   triggler = true;

    //   timer_register->kill_timer(eTimerID::TEST_ID);

    //   STestData* attach2 = new STestData();
    //   attach2->a         = 11;
    //   attach2->b         = 12;
    //   timer_register->add_once_timer(
    //     eTimerID::TEST_ID, delay, [](void* data) {
    //       STestData* pTest = (STestData*)data;
    //       LogInfoA("[timer] test timer exec a = {} b = {}", pTest->a, pTest->b);
    //     },
    //     attach2,
    //     testdata_release);
    // }
  }

  TIMER_RELEASE();
  LOG_RELEASE();
  return 0;
}