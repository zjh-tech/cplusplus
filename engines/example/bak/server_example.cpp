/*
 * @Author: zhengjinhong
 * @Date: 2019-11-13 11:26:15
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-04-13 20:28:31
 * @Description: file content
 */

#include "engine/inc/log/logger.h"
#include "engine/inc/tcp/env.h"
#include <chrono>
#include <thread>

#include <sstream>
#ifdef GPERFTOOLS_CPU_MACRO
#  include <gperftools/profiler.h>
#endif

#ifdef GPERFTOOLS_MEMORY_MACRO
#  include <gperftools/heap-profiler.h>
#endif

#if defined(__linux__)
#  include <signal.h>
void sig_handler(int signo) {
  switch (signo) {
    case SIGUSR1: {
#  ifdef GPERFTOOLS_CPU_MACRO
      static bool cpu_start_flag = false;
      if (!cpu_start_flag) {
        cpu_start_flag = true;
        std::stringstream cpu_ss;
        cpu_ss << "cpu.prof";
        ProfilerStart(cpu_ss.str().c_str());
        LogInfoA("recv sig start cpu collection");
      } else {
        cpu_start_flag = false;
        ProfilerStop();
        LogInfoA("recv sig stop cpu collection");
      }
#  endif

    } break;
    case SIGUSR2: {
#  ifdef GPERFTOOLS_MEMORY_MACRO
      static bool memory_start_flag = false;
      if (!memory_start_flag) {
        memory_start_flag = true;
        std::stringstream memory_ss;
        memory_ss << "memory";
        HeapProfilerStart(memory_ss.str().c_str());
        LogInfoA("recv sig start memory collection");
      } else {
        memory_start_flag = false;
        HeapProfilerDump("recv sig");
        HeapProfilerStop();
        LogInfoA("recv sig stop memory collection");
      }
#  endif

    } break;
  }
}
#endif

void timer_print_qps(int64_t diff_time) {
  std::shared_ptr<asio::steady_timer> qps_timer = std::make_shared<asio::steady_timer>(CMainModule::Instance()->get_io_context());
  std::chrono::milliseconds           time_long{1000 * diff_time};
  qps_timer->expires_after(time_long);
  qps_timer->async_wait([qps_timer, diff_time](asio::error_code ec) {
    if (ec) {
      return;
    }

    CSocket::print_qps(diff_time);
    timer_print_qps(diff_time);
  });
}

#define TOkEN_SIZE 32

struct STestStructReqMsg {
  uint32_t msg_id            = 0;
  char     token[TOkEN_SIZE] = {0};
};

struct STestStructResMsg {
  uint32_t msg_id            = 0;
  char     token[TOkEN_SIZE] = {0};
};

class CTestMsgHandler : public IMsgHandler {
public:
  void handler_msg(ISessionPtr pSesssion, const char* data, uint32_t len) override {
    NetStream        req_stream(data, len);
    STestStructReqMsg req;
    req_stream >> req.msg_id;
    req_stream.ReadBuff(req.token, TOkEN_SIZE);
    //LogInfoA("[Net] Recv STestStructReqMsg");

    STestStructResMsg res;
    res.msg_id = req.msg_id;
    std::memcpy(res.token, req.token, TOkEN_SIZE);
    NetStream res_stream;
    res_stream << res.msg_id;
    res_stream.WriteBuff(res.token, TOkEN_SIZE);
    pSesssion->send(res_stream.Data(), res_stream.Size());
    //LogInfoA("[Net] Send STestStructResMsg");
  }
};

int main(void) {
  LOG_INIT("server_example", true, "debug", true);

  uint32_t thread_num                            = std::thread::hardware_concurrency();
  thread_num                                     = 1;
  std::shared_ptr<ProcessorPool> processor_pool = std::make_shared<ProcessorPool>();
  processor_pool->start(thread_num);
  TCP_INIT();

#if defined(__linux__)
  if (SIG_ERR == signal(SIGUSR1, sig_handler)) {
    return false;
  }

  if (SIG_ERR == signal(SIGUSR2, sig_handler)) {
    return false;
  }
#endif

  CSessionCreateFactory::Instance()->AddRelation<INetEventHandler, CTestMsgHandler, ICodecHandler>(0);

  std::shared_ptr<Listener> listener = std::make_shared<Listener>(processor_pool);
  //listener->start("127.0.0.1", 7000);
  //listener->start("192.168.92.128", 7000);
  listener->start("10.23.119.16", 7000);

  int64_t diff_time = 60;
  timer_print_qps(diff_time);

  bool busy = false;
  while (true) {
    busy = false;
    busy = CMainModule::Instance()->update();

    if (!busy) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }

  listener->stop();
  processor_pool->stop();

  TCP_RELEASE();
  LOG_RELEASE();

  return 0;
}