/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-03 10:47:52
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-12-18 11:33:06
 */

#include "timemeter.h"
#include "engine/inc/common/utils.hpp"
#include "engine/inc/log/env.h"

using namespace Framework;

TimeMeter::TimeMeter(int64_t limit_sec) {
  limit_mill_sec = limit_sec;
}

void TimeMeter::Stamp() {
  int64_t now      = Utils::GetMillTime();
  stamp_vec[index] = now;
  ++index;
}

void TimeMeter::Reset() {
  index = 0;
  buf.clear();
  start_stamp_tick = Utils::GetMillTime();
}

void TimeMeter::CheckOut(const char* file_name, int line) {
  Stamp();

  if (index != 0 && (stamp_vec[index - 1] - start_stamp_tick) >= limit_mill_sec) {
    buf << file_name << ":" << line << " ";

    for (size_t i = 0; i < index; ++i) {
      if (i != 0) {
        buf << "-";
      }
      buf << "[" << (stamp_vec[i] - start_stamp_tick) << "]";
    }
    LogWarnA("[TimeMeter] TimeOut: {}", buf.str());
  }
}