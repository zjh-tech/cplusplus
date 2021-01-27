/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-12-23 18:58:34
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-12-24 15:03:44
 */

#pragma once
#include <asio.hpp>
#include <memory>
#include <thread>
#include <vector>

using namespace std;

namespace Framework {

struct Processor {
  shared_ptr<thread>                 ThreadPtr = nullptr;
  asio::io_context                   IoContext;
  shared_ptr<asio::io_context::work> IoContextWorkPtr = nullptr;
  bool                               RunFlag          = true;
};

class IOContextPool {
public:
  void Start(uint32_t thread_num, uint32_t loop_num);

  void Stop();

  asio::io_context& GetNextIoContext();

private:
  void run(shared_ptr<Processor> processor_ptr, uint32_t loop_num);

private:
  vector<shared_ptr<Processor>> processor_vec;

  size_t       io_context_next_index;
  atomic<bool> start_flag = false;
};

}  // namespace Framework
