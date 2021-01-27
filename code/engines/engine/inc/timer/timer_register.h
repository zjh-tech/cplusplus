/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-15 19:27:22
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-01-06 15:28:57
 */

#pragma once

#include "engine/inc/timer/itimer.h"
#include <any>
#include <tuple>

using namespace std;

namespace Framework {
namespace Timer {
  class TimerRegister : public enable_shared_from_this<TimerRegister>, public ITimerRegister {
    friend class TimeWheelMgr;

  public:
    TimerRegister();
    virtual ~TimerRegister();

  public:
    bool AddOnceTimer(uint32_t register_eid, uint64_t delay, const TimerFunc& cb_func) override;

    bool AddRepeatTimer(uint32_t register_eid, uint64_t delay, const TimerFunc& cb_func) override;

    bool HasTimer(uint32_t register_eid) override;

    void KillTimer(uint32_t register_eid) override;

    tuple<bool, uint64_t> GetRemainTime(uint32_t register_eid) override;

    void KillAllTimer() override;

  private:
    bool add_timer(uint32_t register_eid, uint64_t delay, const TimerFunc& cb_func, bool repeated, bool replace);

    Timer* create_timer(uint32_t register_eid, uint64_t delay, const TimerFunc& cb_func, bool repeat);

    void remove_timer(uint32_t register_eid, uint64_t timer_register_id);

  private:
    map<uint32_t, Timer*> m_map;
    uint64_t              m_timer_register_uid = 0;

  };  // namespace Timer

  extern shared_ptr<ITimerRegister> GTimerRegister;

}  // namespace Timer
}  // namespace Framework