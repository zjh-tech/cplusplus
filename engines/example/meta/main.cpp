/*
 * @Author: zhengjinhong
 * @Date: 2019-11-11 10:44:56
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-11-19 14:48:20
 * @Description: file content
 */

#include "engine/inc/log/logger.h"
#include "engine/inc/tcp/env.h"

int main(void) {
#if defined(__linux__)
  LOG_INIT("./", "log_example", false, 0, true);
#else
  LOG_INIT("./", "log_example", true, 0, true);
#endif

  uint32_t                        thread_num     = std::thread::hardware_concurrency();
  std::shared_ptr<ProcessorPool> processor_pool = std::make_shared<ProcessorPool>();
  processor_pool->Start(thread_num, 100);

  processor_pool->Stop();

  LOG_RELEASE();

  return 0;
}
