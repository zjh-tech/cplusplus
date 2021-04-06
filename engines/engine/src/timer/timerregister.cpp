#include "engine/inc/timer/timerregister.h"

#include <assert.h>

#include "engine/inc/common/marco.hpp"
#include "engine/inc/common/objectpoolmgr.h"
#include "engine/inc/common/utils.hpp"
#include "engine/inc/log/logger.h"
#include "engine/inc/timer/timer.h"
#include "engine/inc/timer/timewheel.h"

namespace Framework
{
    namespace Timer
    {
        shared_ptr<ITimerRegister> GTimerRegister = nullptr;

        TimerRegister::TimerRegister()
        {
        }

        TimerRegister::~TimerRegister()
        {
            KillAllTimer();
        }

        bool TimerRegister::AddOnceTimer(uint32_t register_eid, time_t delay, const TimerFunc& cb_func)
        {
            return add_timer(register_eid, delay, cb_func, false, true);
        }

        bool TimerRegister::AddRepeatTimer(uint32_t register_eid, time_t delay, const TimerFunc& cb_func)
        {
            return add_timer(register_eid, delay, cb_func, true, true);
        }

        bool TimerRegister::add_timer(uint32_t register_eid, time_t delay, const TimerFunc& cb_func, bool repeated, bool replace)
        {
            if ((uint64_t)delay == NOVALID_DELAY_MILL_MARCO)
            {
                return false;
            }

            bool exist = HasTimer(register_eid);

            if (exist && !replace)
            {
                return true;
            }

            if (exist && replace)
            {
                KillTimer(register_eid);
            }

            Timer* timer = create_timer(register_eid, delay, cb_func, repeated);
            assert(timer);
            if (!timer)
            {
                LogErrorA("[timer]  create_timer error");
                return false;
            }

            if (delay == 0)
            {
                LogWarnA("[timer]  Delay = 0");
                timer->cb_func_();
                return true;
            }

            enum_map_[timer->register_eid_] = timer;
            TimeWheelMgr::Instance()->add_slot_timer(timer);
            return true;
        }

        Timer* TimerRegister::create_timer(uint32_t register_eid, time_t delay, const TimerFunc& cb_func, bool repeat)
        {
            Timer* timer = nullptr;

            if (GObjectPoolMgr)
            {
                auto timer_pool = GObjectPoolMgr->GetObjectPool<Timer>();
                if (timer_pool)
                {
                    timer = timer_pool->FetchObj();
                }
            }

            if (!timer)
            {
                timer = new Timer();
            }

            assert(timer);
            if (timer)
            {
                timer->register_eid_ = register_eid;
                ++timer_register_uid_;
                assert(timer_register_uid_ != numeric_limits<uint64_t>::max());
                timer->register_uid_   = timer_register_uid_;
                timer->delay_          = delay;
                timer->cb_func_        = cb_func;
                timer->repeat_flag_    = repeat;
                timer->timer_register_ = this->shared_from_this();
            }

            return timer;
        }

        bool TimerRegister::HasTimer(uint32_t enum_id)
        {
            auto iter = enum_map_.find(enum_id);
            return (iter != enum_map_.end()) ? true : false;
        }

        tuple<bool, time_t> TimerRegister::GetRemainTime(uint32_t register_eid)
        {
            auto iter = enum_map_.find(register_eid);
            if (iter == enum_map_.end() || iter->second == nullptr)
            {
                return make_tuple(false, 0);
            }

            Timer* timer       = iter->second;
            time_t remain_time = 0;
            if (timer->state_ == eTimerState::Running)
            {
                uint32_t cur_slot = TimeWheelMgr::Instance()->get_cur_slot();
                if (timer->slot_ < cur_slot)
                {
                    remain_time = timer->slot_ * TimeWheelMgr::s_slot_max_num + timer->slot_ - cur_slot;
                }
                else
                {
                    remain_time = timer->slot_ * TimeWheelMgr::s_slot_max_num + (TimeWheelMgr::s_slot_max_num - cur_slot + timer->slot_);
                }
            }

            return make_tuple(true, remain_time);
        }

        void TimerRegister::KillTimer(uint32_t enum_id)
        {
            auto iter = enum_map_.find(enum_id);
            if (iter != enum_map_.end())
            {
                if (iter->second)
                {
                    iter->second->state_ = eTimerState::Killed;
                    LogDebugA("[timer]  kill timer enum_id = {},register_id = {}, timewheel_uid ={}", iter->second->register_eid_, iter->second->register_uid_, iter->second->timewheel_uid_);
                }
                enum_map_.erase(iter);
            }
        }

        void TimerRegister::KillAllTimer()
        {
            for (auto iter = enum_map_.begin(); iter != enum_map_.end(); ++iter)
            {
                if (iter->second)
                {
                    iter->second->state_ = eTimerState::Killed;
                    LogDebugA("[timer]  kill timer enum_id = {},register_id = {}, timewheel_uid ={}", iter->second->register_eid_, iter->second->register_uid_, iter->second->timewheel_uid_);
                }
            }
            enum_map_.clear();
        }

        void TimerRegister::remove_timer(uint32_t enum_id, uint64_t timer_register_id)
        {
            auto iter = enum_map_.find(enum_id);
            // same enum_id timer delete after again add :  register_uid not same
            if (iter != enum_map_.end() && (iter->second->register_uid_ == timer_register_id))
            {
                LogDebugA("[timer] TimerRegister remove timer enum_id = {},register_id = {}, timewheel_uid ={}", iter->second->register_eid_, iter->second->register_uid_, iter->second->timewheel_uid_);
                enum_map_.erase(iter);
            }
        }

    }  // namespace Timer
}  // namespace Framework