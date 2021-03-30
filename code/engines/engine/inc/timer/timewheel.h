#pragma once
#include <functional>

#include "engine/inc/common/list.h"
#include "engine/inc/common/singleton.hpp"
#include "engine/inc/timer/itimer.h"

namespace Framework
{
    namespace Timer
    {
        struct Timer;
        using TimerFunc = function<void()>;

        class TimeWheelMgr : public Singleton<TimeWheelMgr>, public ITimeWheelMgr
        {
            friend class TimerRegister;

        public:
            TimeWheelMgr();
            virtual ~TimeWheelMgr();

        public:
            bool Run(int count) override;

            void Stop();

        private:
            inline uint32_t get_cur_slot()
            {
                return cur_slot_;
            }

            void add_slot_timer(Timer* timer);

            void release_timer(Timer* timer);

        private:
            struct list_head* slot_array_ = nullptr;  // s_slot_num array save
            uint32_t cur_slot_            = 0;
            uint64_t last_time_           = 0;
            uint64_t timewheel_uid_       = 0;

            static const int32_t s_slot_max_num        = 1000 * 60;
            static const uint64_t s_slot_interval_time = 1;
        };

    }  // namespace Timer
}  // namespace Framework