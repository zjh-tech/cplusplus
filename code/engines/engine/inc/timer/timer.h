/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-15 19:27:33
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-12-29 16:25:43
 */

#pragma once

#include "engine/inc/common/list.h"
#include <any>
#include <cinttypes>
#include <functional>
#include <memory>
#include <vector>

using namespace std;

namespace Framework {
namespace Timer {

  class TimerRegister;
  using TimerRegisterPtr = shared_ptr<TimerRegister>;

  using TimerFunc = function<void()>;

  enum class eTimerState {
    Invalid = 1,
    Running = 2,
    Killed  = 3,
  };

  struct Timer {
    uint32_t         register_eid   = 0;
    uint64_t         register_uid   = 0;
    uint64_t         timewheel_uid  = 0;
    uint64_t         delay          = 0;
    int32_t          rotation       = 0;
    uint32_t         slot           = 0;
    eTimerState      state          = eTimerState::Invalid;
    bool             repeat         = false;
    TimerFunc        cb_func        = nullptr;  //lambda
    TimerRegisterPtr timer_register = nullptr;
    struct list_head list;
  };

}  // namespace Timer
}  // namespace Framework