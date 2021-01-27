/*
 * @Author: zhengjinhong
 * @Date: 2019-11-11 10:44:56
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-10-30 18:49:37
 * @Description: file content
 */

#include "engine/inc/log/logger.h"

int main(void) {
#if defined(__linux__)
  LOG_INIT("./", "log_example", false, 0, true);
#else
  LOG_INIT("./", "log_example", true, 0, true);
#endif

  LogDebugA("debug name = {},id = {}", "zjh", 2);
  LogInfoA("info name = {},id = {}", "zjh", 3);
  LogWarnA("warn name = {},id = {}", "zjh", 4);
  LogErrorA("error name = {},id = {}", "zjh", 5);
  LogError("critical name = {},id = {}", "zjh", 6);

  LOG_RELEASE();

  return 0;
}
