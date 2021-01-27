/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-16 17:01:44
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-01-07 12:04:29
 */

#pragma once

#include "engine/inc/common/marco.hpp"
#include "engine/inc/timer/itimer.h"
#include "engine/inc/timer/timer.h"
#include "engine/inc/timer/timer_register.h"
#include "engine/inc/timer/timewheel.h"

using namespace Framework;
using namespace Framework::Timer;
using namespace std;

#define TIMER_INIT()        \
  TimeWheelMgr::Instance(); \
  GTimerRegister = make_shared<TimerRegister>();

#define TIMER_UNINIT()              \
  TimeWheelMgr::Instance()->Stop(); \
  if (GTimerRegister) {             \
    GTimerRegister = nullptr;       \
  }

//定时器回调带函数设计方案:
//1  use lambda   == >  lambda can catch attach paras
//2  vector<any>  == > save attach paras