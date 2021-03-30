#include "engine/inc/timer/timewheel.h"

#include <assert.h>

#include "engine/inc/common/objectpoolmgr.h"
#include "engine/inc/common/utils.hpp"
#include "engine/inc/log/logger.h"
#include "engine/inc/timer/timer.h"
#include "engine/inc/timer/timerregister.h"

using namespace Framework;

namespace Framework
{
    namespace Timer
    {
        TimeWheelMgr::TimeWheelMgr()
        {
            slot_array_ = new list_head[s_slot_max_num];
            assert(slot_array_);

            for (int32_t i = 0; i < s_slot_max_num; ++i)
            {
                INIT_LIST_HEAD(&slot_array_[i]);
            }

            last_time_ = Utils::GetMillTime();
        }

        TimeWheelMgr::~TimeWheelMgr()
        {
            if (!slot_array_)
            {
                return;
            }

            list_head *pos, *n = nullptr;
            Timer* timer = nullptr;
            for (uint32_t i = 0; i < s_slot_max_num; ++i)
            {
                list_head* cur_slot_list = &slot_array_[cur_slot_];
                list_for_each_safe(pos, n, cur_slot_list)
                {
                    timer = list_entry(pos, Timer, list_);
                    if (timer)
                    {
                        release_timer(timer);
                        list_del_init(pos);
                    }
                }
            }

            delete[] slot_array_;
            slot_array_ = nullptr;
        }

        bool TimeWheelMgr::Run(int count)
        {
            uint64_t cur_mill_sec = Utils::GetMillTime();
            if (cur_mill_sec < last_time_)
            {
                LogErrorA("[Timer] Time Rollback");
                return false;
            }

            bool busy = false;

            uint64_t delta = cur_mill_sec - last_time_;
            if (delta > (uint64_t)count)
            {
                delta = (uint64_t)count;
                LogWarnA("[Timer] Time Forward");
            }

            last_time_ = cur_mill_sec;

            list_head *pos, *n = nullptr;
            Timer* timer = nullptr;

            for (; delta > 0; --delta)
            {
                ++cur_slot_;
                cur_slot_ = cur_slot_ % s_slot_max_num;
                // LogDebugA("[timer] cur_slot = {}", cur_slot_);

                list_head* cur_slot_list = &slot_array_[cur_slot_];
                list_head* repeat_list   = new list_head();
                INIT_LIST_HEAD(repeat_list);
                list_for_each_safe(pos, n, cur_slot_list)
                {
                    timer = list_entry(pos, Timer, list_);
                    assert(timer);
                    if (!timer)
                    {
                        continue;
                    }

                    if (timer->state_ != eTimerState::Running)
                    {
                        release_timer(timer);
                        list_del_init(pos);
                        continue;
                    }

                    --timer->rotation_;
                    if (timer->rotation_ < 0)
                    {
                        busy = true;
                        LogDebugA("[timer]  triggle timer id {}-{}-{}", timer->register_eid_, timer->register_uid_,
                                  timer->timewheel_uid_);
                        timer->cb_func_();
                        list_del_init(pos);
                        if (timer->repeat_flag_)
                        {
                            list_add_tail(&timer->list_, repeat_list);
                            ////先加入repeat_list,防止此循环又被遍历到
                        }
                        else
                        {
                            release_timer(timer);
                        }
                    }
                    else
                    {
                        LogDebugA("[timer] remain rotation = {} remain sec ={}", timer->rotation_,
                                  timer->rotation_ * s_slot_max_num);
                    }
                }

                pos = nullptr;
                n   = nullptr;
                list_for_each_safe(pos, n, repeat_list)
                {
                    timer = list_entry(pos, Timer, list_);
                    assert(timer);
                    if (!timer)
                    {
                        continue;
                    }
                    add_slot_timer(timer);
                }
            }

            return busy;
        }

        void TimeWheelMgr::Stop()
        {
            LogInfoA("[Timer] Stop");
        }

        void TimeWheelMgr::add_slot_timer(Timer* timer)
        {
            assert(timer && timer->timer_register_);
            if (!timer || !timer->timer_register_)
            {
                LogErrorA("[timer] add_slot_timer error");
                return;
            }

            // calc rotation and slot
            timer->rotation_ = timer->delay_ / s_slot_max_num;
            timer->slot_     = (cur_slot_ + timer->delay_ % s_slot_max_num) % s_slot_max_num;
            if (timer->slot_ == cur_slot_ && timer->rotation_ > 0)
            {
                timer->rotation_ -= 1;
            }
            timer->state_ = eTimerState::Running;
            ++timewheel_uid_;
            timer->timewheel_uid_ = timewheel_uid_;
            list_add_tail(&timer->list_, &slot_array_[timer->slot_]);

            uint64_t triggler_time = Utils::GetMillTime() + timer->delay_;
            LogDebugA("[timer]  add timer id {}-{}-{} delay ={},rotation = {} curslot = {} slot = {},triggle time = {}",
                      timer->register_eid_, timer->register_uid_, timer->timewheel_uid_, timer->delay_,
                      timer->rotation_, cur_slot_, timer->slot_, triggler_time);
        }

        void TimeWheelMgr::release_timer(Timer* timer)
        {
            assert(timer);
            if (timer)
            {
                if (timer->state_ == eTimerState::Running)
                {
                    LogDebugA("[timer] TimeWheel release id {}-{}-{} state = Running", timer->register_eid_,
                              timer->register_uid_, timer->timewheel_uid_);
                }
                else if (timer->state_ == eTimerState::Killed)
                {
                    LogDebugA("[timer] TimeWheel release id {}-{}-{} state = Killed", timer->register_eid_,
                              timer->register_uid_, timer->timewheel_uid_);
                }
                else
                {
                    assert(false);
                    LogDebugA("[timer] TimeWheel release id {}-{}-{} state = Unknown", timer->register_eid_,
                              timer->register_uid_, timer->timewheel_uid_);
                }

                if (timer->timer_register_)
                {
                    timer->timer_register_->remove_timer(timer->register_eid_, timer->register_uid_);
                    timer->timer_register_ = nullptr;
                }
            }

            if (GObjectPoolMgr)
            {
                auto timer_pool = GObjectPoolMgr->GetObjectPool<Timer>();
                if (timer_pool)
                {
                    timer_pool->ReleaseObj(timer);
                }
            }
            else
            {
                delete timer;
                timer = nullptr;
            }
        }

    }  // namespace Timer
}  // namespace Framework