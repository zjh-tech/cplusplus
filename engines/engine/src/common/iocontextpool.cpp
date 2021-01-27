/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-12-23 18:58:47
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-01-07 11:28:26
 */

#include "engine/inc/common/iocontextpool.h"
#include "engine/inc/common/utils.hpp"
#include "engine/inc/log/env.h"

namespace Framework {

void IOContextPool::Start(uint32_t thread_num, uint32_t loop_num) {
  bool old_start_flag = false;
  if (start_flag.compare_exchange_strong(old_start_flag, true) == false) {
    return;
  }

  for (uint32_t i = 0; i < thread_num; ++i) {
    auto processor              = make_shared<Processor>();
    processor->IoContextWorkPtr = make_shared<asio::io_context::work>(processor->IoContext);
    processor->ThreadPtr        = make_shared<thread>([this, processor, loop_num]() {
      run(processor, loop_num);
    });
    processor_vec.push_back(processor);
  }
}

void IOContextPool::Stop() {
  bool old_start_flag = true;
  if (start_flag.compare_exchange_strong(old_start_flag, false) == false) {
    return;
  }

  size_t total_num = processor_vec.size();
  for (size_t i = 0; i < total_num; ++i) {
    processor_vec[i]->RunFlag = false;
  }

  for (size_t i = 0; i < total_num; ++i) {
    processor_vec[i]->ThreadPtr->join();
  }
}

asio::io_context& IOContextPool::GetNextIoContext() {
  if (!processor_vec.empty()) {
    io_context_next_index = (io_context_next_index + 1) % processor_vec.size();
    return processor_vec[io_context_next_index]->IoContext;
  }
  assert(false);
  LogErrorA("IOContextPool GetNextIoContext Error");
  static asio::io_context                   io_context;
  static shared_ptr<asio::io_context::work> io_contextwork_ptr = make_shared<asio::io_context::work>(io_context);
  return io_context;
}

void IOContextPool::run(shared_ptr<Processor> processor_ptr, uint32_t loop_num) {
  LogInfo("Start IoContextPool Thread ID={}", Utils::GetThreadId());

  //io_context_ptr.run();

  auto& io_context = processor_ptr->IoContext;
  bool  busy       = false;
  while (processor_ptr->RunFlag) {
    busy = false;
    if (io_context.poll_one() != 0) {
      busy = true;
      for (uint32_t i = 0; i < loop_num; ++i) {
        if (io_context.poll_one() == 0) {
          break;
        }
      }
    }

    if (!busy) {
      this_thread::sleep_for(chrono::milliseconds(1));
    }
  }
  LogInfo("Stop IoContextPool Thread ID={}", Utils::GetThreadId());
}

}  // namespace Framework
