/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-15 19:30:12
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-02-07 17:22:21
 */

#include "engine/inc/timer/timewheel.h"
#include "engine/inc/common/objectpoolmgr.h"
#include "engine/inc/common/utils.hpp"
#include "engine/inc/log/logger.h"
#include "engine/inc/timer/timer.h"
#include "engine/inc/timer/timerregister.h"
#include <assert.h>

using namespace Framework;

namespace Framework {
namespace Timer {

  TimeWheelMgr::TimeWheelMgr() {
    m_slot_array = new list_head[s_slot_max_num];
    assert(m_slot_array);

    for (int32_t i = 0; i < s_slot_max_num; ++i) {
      INIT_LIST_HEAD(&m_slot_array[i]);
    }

    m_last_time = Utils::GetMillTime();
  }

  TimeWheelMgr::~TimeWheelMgr() {
    if (!m_slot_array) {
      return;
    }

    list_head *pos, *n = nullptr;
    Timer*     timer = nullptr;
    for (uint32_t i = 0; i < s_slot_max_num; ++i) {
      list_head* cur_slot_list = &m_slot_array[m_cur_slot];
      list_for_each_safe(pos, n, cur_slot_list) {
        timer = list_entry(pos, Timer, list);
        if (timer) {
          release_timer(timer);
          list_del_init(pos);
        }
      }
    }

    delete[] m_slot_array;
    m_slot_array = nullptr;
  }

  bool TimeWheelMgr::Run(int count) {
    uint64_t cur_mill_sec = Utils::GetMillTime();
    if (cur_mill_sec < m_last_time) {
      LogErrorA("[Timer] Time Rollback");
      return false;
    }

    bool busy = false;

    uint64_t delta = cur_mill_sec - m_last_time;
    if (delta > (uint64_t)count) {
      delta = (uint64_t)count;
      LogWarnA("[Timer] Time Forward");
    }

    m_last_time = cur_mill_sec;

    list_head *pos, *n = nullptr;
    Timer*     timer = nullptr;

    for (; delta > 0; --delta) {
      ++m_cur_slot;
      m_cur_slot = m_cur_slot % s_slot_max_num;
      //LogDebugA("[timer] cur_slot = {}", m_cur_slot);

      list_head* cur_slot_list = &m_slot_array[m_cur_slot];
      list_head* repeat_list   = new list_head();
      INIT_LIST_HEAD(repeat_list);
      list_for_each_safe(pos, n, cur_slot_list) {
        timer = list_entry(pos, Timer, list);
        assert(timer);
        if (!timer) {
          continue;
        }

        if (timer->state != eTimerState::Running) {
          release_timer(timer);
          list_del_init(pos);
          continue;
        }

        --timer->rotation;
        if (timer->rotation < 0) {
          busy = true;
          LogDebugA("[timer]  triggle timer id {}-{}-{}", timer->register_eid, timer->register_uid, timer->timewheel_uid);
          timer->cb_func();
          list_del_init(pos);
          if (timer->repeat) {
            list_add_tail(&timer->list, repeat_list);
            ////先加入repeat_list,防止此循环又被遍历到
          } else {
            release_timer(timer);
          }
        } else {
          LogDebugA("[timer] remain rotation = {} remain sec ={}", timer->rotation, timer->rotation * s_slot_max_num);
        }
      }

      pos = nullptr;
      n   = nullptr;
      list_for_each_safe(pos, n, repeat_list) {
        timer = list_entry(pos, Timer, list);
        assert(timer);
        if (!timer) {
          continue;
        }
        add_slot_timer(timer);
      }
    }

    return busy;
  }

  void TimeWheelMgr::Stop() {
    LogInfoA("[Timer] Stop");
  }

  void TimeWheelMgr::add_slot_timer(Timer* timer) {
    assert(timer && timer->timer_register);
    if (!timer || !timer->timer_register) {
      LogErrorA("[timer] add_slot_timer error");
      return;
    }

    //calc rotation and slot
    timer->rotation = timer->delay / s_slot_max_num;
    timer->slot     = (m_cur_slot + timer->delay % s_slot_max_num) % s_slot_max_num;
    if (timer->slot == m_cur_slot && timer->rotation > 0) {
      timer->rotation -= 1;
    }
    timer->state = eTimerState::Running;
    ++m_timewheel_uid;
    timer->timewheel_uid = m_timewheel_uid;
    list_add_tail(&timer->list, &m_slot_array[timer->slot]);

    uint64_t triggler_time = Utils::GetMillTime() + timer->delay;
    LogDebugA("[timer]  add timer id {}-{}-{} delay ={},rotation = {} curslot = {} slot = {},triggle time = {}", timer->register_eid, timer->register_uid, timer->timewheel_uid, timer->delay, timer->rotation, m_cur_slot, timer->slot, triggler_time);
  }

  void TimeWheelMgr::release_timer(Timer* timer) {
    assert(timer);
    if (timer) {
      if (timer->state == eTimerState::Running) {
        LogDebugA("[timer] TimeWheel release id {}-{}-{} state = Running", timer->register_eid, timer->register_uid, timer->timewheel_uid);
      } else if (timer->state == eTimerState::Killed) {
        LogDebugA("[timer] TimeWheel release id {}-{}-{} state = Killed", timer->register_eid, timer->register_uid, timer->timewheel_uid);
      } else {
        assert(false);
        LogDebugA("[timer] TimeWheel release id {}-{}-{} state = Unknown", timer->register_eid, timer->register_uid, timer->timewheel_uid);
      }

      if (timer->timer_register) {
        timer->timer_register->remove_timer(timer->register_eid, timer->register_uid);
        timer->timer_register = nullptr;
      }
    }

    if (GObjectPoolMgr) {
      auto timer_pool = GObjectPoolMgr->GetObjectPool<Timer>();
      if (timer_pool) {
        timer_pool->ReleaseObj(timer);
      }
    } else {
      delete timer;
      timer = nullptr;
    }
  }

}  // namespace Timer
}  // namespace Framework