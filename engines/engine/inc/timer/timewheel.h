/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-15 19:25:21
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-01-07 11:53:06
 */

#pragma once
#include "engine/inc/common/list.h"
#include "engine/inc/common/singleton.hpp"
#include "engine/inc/timer/itimer.h"
#include <functional>

namespace Framework {
namespace Timer {

  struct Timer;
  using TimerFunc = function<void()>;

  class TimeWheelMgr : public Singleton<TimeWheelMgr>, public ITimeWheelMgr {
    friend class TimerRegister;

  public:
    TimeWheelMgr();
    virtual ~TimeWheelMgr();

  public:
    bool Run(int count) override;

    void Stop();

  private:
    inline uint32_t get_cur_slot() {
      return m_cur_slot;
    }

    void add_slot_timer(Timer* timer);

    void release_timer(Timer* timer);

  private:
    struct list_head* m_slot_array    = nullptr;  //s_slot_num array save
    uint32_t          m_cur_slot      = 0;
    uint64_t          m_last_time     = 0;
    uint64_t          m_timewheel_uid = 0;

    static const int32_t  s_slot_max_num       = 1000 * 60;
    static const uint64_t s_slot_interval_time = 1;
  };

}  // namespace Timer
}  // namespace Framework