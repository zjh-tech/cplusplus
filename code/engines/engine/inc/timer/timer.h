#pragma once

#include <any>
#include <cinttypes>
#include <functional>
#include <memory>
#include <vector>

#include "engine/inc/common/list.h"

using namespace std;

namespace Framework
{
    namespace Timer
    {
        class TimerRegister;
        using TimerRegisterPtr = shared_ptr<TimerRegister>;

        using TimerFunc = function<void()>;

        enum class eTimerState
        {
            Invalid = 1,
            Running = 2,
            Killed  = 3,
        };

        struct Timer
        {
            uint32_t register_eid_           = 0;
            uint64_t register_uid_           = 0;
            uint64_t timewheel_uid_          = 0;
            time_t delay_                    = 0;
            int32_t rotation_                = 0;
            uint32_t slot_                   = 0;
            eTimerState state_               = eTimerState::Invalid;
            bool repeat_flag_                = false;
            TimerFunc cb_func_               = nullptr;  // lambda
            TimerRegisterPtr timer_register_ = nullptr;
            struct list_head list_;
        };

    }  // namespace Timer
}  // namespace Framework