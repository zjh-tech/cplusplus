#include <iostream>

#include "engine/inc/log/env.h"

int main(void)
{
    LOG_INIT("./log_log", "log_example", 1);

    GLogger->qps_loop_count = 10000000;
    uint32_t loop_num       = GLogger->qps_loop_count;
    for (uint32_t i = 0; i < loop_num; ++i)
    {
        LogInfoA("This message is 116 characters long including the info that comes before it. {}", i);
    }

    bool busy = false;
    while (true)
    {
        busy = false;

        if (GLogger->qps_end_tick != 0)
        {
            LogInfoA("Sync Qps={}", loop_num * 1000 / (GLogger->qps_end_tick - GLogger->qps_start_tick));
            GLogger->qps_end_tick = 0;
        }

        if (!busy)
        {
            this_thread::sleep_for(chrono::milliseconds(1));
        }
    }

    LOG_UNINIT();

    return 0;
}
