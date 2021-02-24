/*
 * @Author: zhengjinhong
 * @Date: 2019-11-11 10:44:56
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-02-23 19:23:29
 * @Description: file content
 */

#include "engine/inc/log/env.h"

static int64_t GetMillTime() {
  return chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
}

int main(void) {
#if defined(__linux__)
  LOG_INIT("./log_log", "log_example", false, 1);
#else
  LOG_INIT("./log_log", "log_example", true, 1);
#endif

  uint32_t loop_num   = 1000000;
  int64_t  start_tick = GetMillTime();
  for (uint32_t i = 0; i < loop_num; ++i) {
    LogInfo("This message is 116 characters long including the info that comes before it. {}", i);
    //LogInfo("");
  }
  int64_t end_tick = GetMillTime();
  LogInfo("Sync Qps={}", loop_num * 1000 / (end_tick - start_tick));

  bool busy = false;
  while (true) {
    busy = false;

    if (!busy) {
      this_thread::sleep_for(chrono::milliseconds(1));
    }
  }

  LOG_UNINIT();

  return 0;
}
