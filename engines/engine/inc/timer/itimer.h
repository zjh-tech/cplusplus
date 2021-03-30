#pragma once

#include <any>
#include <functional>
#include <map>
#include <memory>
#include <tuple>
#include <vector>

using namespace std;

namespace Framework
{
    namespace Timer
    {
        struct Timer;
        // TimerFunc with paras must be is lambda, lambda can catch attach paras
        using TimerFunc = function<void()>;

        class ITimerRegister
        {
        public:
            virtual bool AddOnceTimer(uint32_t register_eid, time_t delay, const TimerFunc& cb_func)   = 0;
            virtual bool AddRepeatTimer(uint32_t register_eid, time_t delay, const TimerFunc& cb_func) = 0;
            virtual bool HasTimer(uint32_t register_eid)                                               = 0;
            virtual void KillTimer(uint32_t register_eid)                                              = 0;
            virtual tuple<bool, time_t> GetRemainTime(uint32_t register_eid)                           = 0;
            virtual void KillAllTimer()                                                                = 0;
        };

        class ITimeWheelMgr
        {
        public:
            virtual bool Run(int count) = 0;
        };

    }  // namespace Timer
}  // namespace Framework