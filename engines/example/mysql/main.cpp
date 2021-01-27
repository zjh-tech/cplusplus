/*
 * @Author: zhengjinhong
 * @Date: 2019-11-11 10:44:56
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-11-25 10:37:37
 * @Description: file content
 */

#include "engine/inc/log/logger.h"
#include "engine/inc/mysql/env.h"

int main(void) {
#if defined(__linux__)
  LOG_INIT("./", "log_example", false, 0, true);
#else
  LOG_INIT("./", "log_example", true, 0, true);
#endif

  LOG_RELEASE();

  return 0;
}
