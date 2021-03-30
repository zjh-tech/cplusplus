#include "calcdelay.h"
#include "engine/inc/common/utils.hpp"
#include "engine/inc/log/env.h"
#include "engine/inc/timer/env.h"

#define TIMER_LOOP_COUNT 60000

class TestTimer
{
public:
    void timer_min()
    {
        LogInfoA("timer_min exec");
    }

    void test_min()
    {
        int min                = 5;
        uint64_t min_timestamp = CalcDelay::GetNearestDelayEveryMin(min);
        timer_register->AddOnceTimer(eTimerID::EVERY_SOME_MIN_CLOCK, min_timestamp, [min, this]() {
            timer_min();

            timer_register->AddRepeatTimer(eTimerID::EVERY_SOME_MIN_CLOCK, ONE_MIN_MILL_MACRO * min,
                                           [this]() { timer_min(); });
        });
    }

    void timer_hour()
    {
        LogInfoA("timer_hour exec");
    }

    void test_hour()
    {
        int hour                = 1;
        uint64_t hour_timestamp = CalcDelay::GetNearestDelayEveryDay(hour, 10);
        timer_register->AddOnceTimer(eTimerID::EVERY_SOME_HOUR_CLOCK, hour_timestamp, [hour, this]() {
            timer_hour();

            timer_register->AddRepeatTimer(eTimerID::EVERY_SOME_HOUR_CLOCK, ONE_HOUR_MILL_MACRO * hour,
                                           [this]() { timer_hour(); });
        });
    }

    void timer_wday()
    {
        LogInfoA("timer_wday exec");
    }

    void test_wday()
    {
        int wday                = 3;
        uint64_t wday_timestamp = CalcDelay::GetNearestDelayEveryWeek(wday, 10, 10);
        timer_register->AddOnceTimer(eTimerID::EVERY_SOME_WDAY_CLOCK, wday_timestamp, [wday, this]() {
            timer_wday();

            timer_register->AddRepeatTimer(eTimerID::EVERY_SOME_WDAY_CLOCK, ONE_DAY_MILL_MACRO * 7,
                                           [this]() { timer_wday(); });
        });
    }

    void timer_month()
    {
        LogInfoA("timer_month exec");
    }

    void test_month()
    {
        uint64_t month_timestamp = CalcDelay::GetNearestDelayEveryMonth(23, 10, 10);
        timer_register->AddOnceTimer(eTimerID::EVERY_SOME_MONTH_CLOCK, month_timestamp, [this]() {
            timer_month();
            time_t cur_time     = Utils::GetSecTime();
            std::tm* tm_cur_ptr = std::localtime(&cur_time);

            tuple<bool, uint64_t> temp_tuple = Utils::GetTotalDayCount(tm_cur_ptr->tm_mon, tm_cur_ptr->tm_year);
            if (get<0>(temp_tuple) == false)
            {
                return;
            }
            uint64_t tatal_day_count = get<1>(temp_tuple);
            timer_register->AddOnceTimer(eTimerID::EVERY_SOME_MONTH_CLOCK, ONE_MONTH_MILL_MACRO(tatal_day_count),
                                         [this]() { timer_month(); });
        });
    }

private:
    enum eTimerID
    {
        EVERY_SOME_MIN_CLOCK   = 1,  // everyhour min
        EVERY_SOME_HOUR_CLOCK  = 2,  // everyday hour  min
        EVERY_SOME_WDAY_CLOCK  = 3,  // everyweek hour min
        EVERY_SOME_MONTH_CLOCK = 4,  // everymonth day min
    };

    shared_ptr<ITimerRegister> timer_register = make_shared<TimerRegister>();
};

// AddOnceTimer  AddRepeatTimer
// callback cost too long time ==> modify delay

int main(void)
{
#if defined(__linux__)
    LOG_INIT("./timer_log", "timer_example", 1);
#else
    LOG_INIT("./timer_log", "timer_example", 1);
#endif

    TIMER_INIT();

    TestTimer test;
    test.test_min();
    // test.test_hour();
    // test.test_wday();
    // test.test_month();

    bool busy         = false;
    auto timer_module = TimeWheelMgr::Instance();
    while (true)
    {
        busy = false;

        if (timer_module->Run(TIMER_LOOP_COUNT))
        {
            busy = true;
        }

        if (!busy)
        {
            this_thread::sleep_for(chrono::milliseconds(1));
        }
    }

    TIMER_UNINIT();

    LOG_UNINIT();
    return 0;
}