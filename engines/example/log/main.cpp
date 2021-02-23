/*
 * @Author: zhengjinhong
 * @Date: 2019-11-11 10:44:56
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-02-23 17:30:19
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
  int64_t  qps_count  = 0;
  for (uint32_t i = 0; i < loop_num; ++i) {
    LogInfo("DebugA");
    //LogInfo("info name = {},id = {}", "zjh", 3);

    ++qps_count;
    int64_t end_tick = GetMillTime();
    if ((end_tick - start_tick) >= 1000) {
      LogInfo("Sync Qps={}", qps_count);
      qps_count  = 0;
      start_tick = end_tick;
    }
  }

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
