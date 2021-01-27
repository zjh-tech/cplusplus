/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-10-14 17:04:35
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-12-18 16:19:46
 */
#include "engine/inc/log/env.h"

int main(void) {
#if defined(__linux__)
  LOG_INIT("./log", "mete", false, 1);
#else
  LOG_INIT("./log", "mete", true, 1);
#endif

  LOG_UNINIT();

  return 0;
}