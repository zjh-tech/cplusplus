#pragma once

#include <any>
#include <tuple>

#include "engine/inc/timer/itimer.h"

using namespace std;

namespace Framework
{
    namespace Timer
    {
        class TimerRegister : public enable_shared_from_this<TimerRegister>, public ITimerRegister
        {
            friend class TimeWheelMgr;

        public:
            TimerRegister();
            virtual ~TimerRegister();

        public:
            bool AddOnceTimer(uint32_t register_eid, time_t delay, const TimerFunc& cb_func) override;

            bool AddRepeatTimer(uint32_t register_eid, time_t delay, const TimerFunc& cb_func) override;

            bool HasTimer(uint32_t register_eid) override;

            void KillTimer(uint32_t register_eid) override;

            tuple<bool, time_t> GetRemainTime(uint32_t register_eid) override;

            void KillAllTimer() override;

        private:
            bool add_timer(uint32_t register_eid, time_t delay, const TimerFunc& cb_func, bool repeated, bool replace);

            Timer* create_timer(uint32_t register_eid, time_t delay, const TimerFunc& cb_func, bool repeat);

            void remove_timer(uint32_t register_eid, uint64_t timer_register_id);

        private:
            map<uint32_t, Timer*> enum_map_;
            uint64_t timer_register_uid_ = 0;

        };  // namespace Timer

        extern shared_ptr<ITimerRegister> GTimerRegister;

    }  // namespace Timer
}  // namespace Framework