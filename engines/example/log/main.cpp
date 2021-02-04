/*
 * @Author: zhengjinhong
 * @Date: 2019-11-11 10:44:56
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-02-03 15:50:42
 * @Description: file content
 */

#include "engine/inc/log/env.h"

int main(void) {
#if defined(__linux__)
  LOG_INIT("./log_log", "log_example", false, 1);
#else
  LOG_INIT("./log_log", "log_example", true, 1);
#endif

  LogDebugA("debug name = {},id = {}", "zjh", 2);
  LogInfoA("info name = {},id = {}", "zjh", 3);
  LogWarnA("warn name = {},id = {}", "zjh", 4);
  LogErrorA("error name = {},id = {}", "zjh", 5);
  LogError("critical name = {},id = {}", "zjh", 6);

  LOG_UNINIT();

  return 0;
}
