/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-15 19:30:23
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-02-07 17:29:35
 */

#include "engine/inc/timer/timerregister.h"
#include "engine/inc/common/marco.hpp"
#include "engine/inc/common/objectpoolmgr.h"
#include "engine/inc/common/utils.hpp"
#include "engine/inc/log/logger.h"
#include "engine/inc/timer/timer.h"
#include "engine/inc/timer/timewheel.h"
#include <assert.h>

namespace Framework {
namespace Timer {

  shared_ptr<ITimerRegister> GTimerRegister = nullptr;

  TimerRegister::TimerRegister() {
  }

  TimerRegister::~TimerRegister() {
    KillAllTimer();
  }

  bool TimerRegister::AddOnceTimer(uint32_t register_eid, uint64_t delay, const TimerFunc& cb_func) {
    return add_timer(register_eid, delay, cb_func, false, true);
  }

  bool TimerRegister::AddRepeatTimer(uint32_t register_eid, uint64_t delay, const TimerFunc& cb_func) {
    return add_timer(register_eid, delay, cb_func, true, true);
  }

  bool TimerRegister::add_timer(uint32_t register_eid, uint64_t delay, const TimerFunc& cb_func, bool repeated, bool replace) {
    if (delay == NOVALID_DELAY_MILL_MARCO) {
      return false;
    }

    bool exist = HasTimer(register_eid);

    if (exist && !replace) {
      return true;
    }

    if (exist && replace) {
      KillTimer(register_eid);
    }

    Timer* timer = create_timer(register_eid, delay, cb_func, repeated);
    assert(timer);
    if (!timer) {
      LogErrorA("[timer]  create_timer error");
      return false;
    }

    if (delay == 0) {
      LogWarnA("[timer]  Delay = 0");
      timer->cb_func();
      return true;
    }

    m_map[timer->register_eid] = timer;
    TimeWheelMgr::Instance()->add_slot_timer(timer);
    return true;
  }

  Timer* TimerRegister::create_timer(uint32_t register_eid, uint64_t delay, const TimerFunc& cb_func, bool repeat) {
    Timer* timer = nullptr;

    if (GObjectPoolMgr) {
      auto timer_pool = GObjectPoolMgr->GetObjectPool<Timer>();
      if (timer_pool) {
        timer = timer_pool->FetchObj();
      }
    }

    if (!timer) {
      timer = new Timer();
    }

    assert(timer);
    if (timer) {
      timer->register_eid = register_eid;
      ++m_timer_register_uid;
      assert(m_timer_register_uid != numeric_limits<uint64_t>::max());
      timer->register_uid   = m_timer_register_uid;
      timer->delay          = delay;
      timer->cb_func        = cb_func;
      timer->repeat         = repeat;
      timer->timer_register = this->shared_from_this();
    }

    return timer;
  }

  bool TimerRegister::HasTimer(uint32_t enum_id) {
    auto iter = m_map.find(enum_id);
    return (iter != m_map.end()) ? true : false;
  }

  tuple<bool, uint64_t> TimerRegister::GetRemainTime(uint32_t enum_id) {
    auto iter = m_map.find(enum_id);
    if (iter == m_map.end() || iter->second == nullptr) {
      return make_tuple(false, 0);
    }

    Timer*   timer       = iter->second;
    uint64_t remain_time = 0;
    if (timer->state == eTimerState::Running) {
      uint32_t cur_slot = TimeWheelMgr::Instance()->get_cur_slot();
      if (timer->slot < cur_slot) {
        remain_time = timer->slot * TimeWheelMgr::s_slot_max_num + timer->slot - cur_slot;
      } else {
        remain_time = timer->slot * TimeWheelMgr::s_slot_max_num + (TimeWheelMgr::s_slot_max_num - cur_slot + timer->slot);
      }
    }

    return make_tuple(true, remain_time);
  }

  void TimerRegister::KillTimer(uint32_t enum_id) {
    auto iter = m_map.find(enum_id);
    if (iter != m_map.end()) {
      if (iter->second) {
        iter->second->state = eTimerState::Killed;
        LogDebugA("[timer]  kill timer enum_id = {},register_id = {}, timewheel_uid ={}", iter->second->register_eid, iter->second->register_uid, iter->second->timewheel_uid);
      }
      m_map.erase(iter);
    }
  }

  void TimerRegister::KillAllTimer() {
    for (auto iter = m_map.begin(); iter != m_map.end(); ++iter) {
      if (iter->second) {
        iter->second->state = eTimerState::Killed;
        LogDebugA("[timer]  kill timer enum_id = {},register_id = {}, timewheel_uid ={}", iter->second->register_eid, iter->second->register_uid, iter->second->timewheel_uid);
      }
    }
    m_map.clear();
  }

  void TimerRegister::remove_timer(uint32_t enum_id, uint64_t timer_register_id) {
    auto iter = m_map.find(enum_id);
    //same enum_id timer delete after again add :  register_uid not same
    if (iter != m_map.end() && (iter->second->register_uid == timer_register_id)) {
      LogDebugA("[timer] TimerRegister remove timer enum_id = {},register_id = {}, timewheel_uid ={}", iter->second->register_eid, iter->second->register_uid, iter->second->timewheel_uid);
      m_map.erase(iter);
    }
  }

}  // namespace Timer
}  // namespace Framework