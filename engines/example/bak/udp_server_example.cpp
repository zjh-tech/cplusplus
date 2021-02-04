/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-03-31 17:51:33
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-04-13 20:26:43
 */

#include "engine/inc/log/logger.h"
#include "engine/inc/udp/env.h"

using namespace Framework;
using namespace Framework::Udp;

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
    LogInfoA("[Net] Recv STestStructReqMsg");

    STestStructResMsg res;
    res.msg_id = req.msg_id;
    std::memcpy(res.token, req.token, TOkEN_SIZE);
    NetStream res_stream;
    res_stream << res.msg_id;
    res_stream.WriteBuff(res.token, TOkEN_SIZE);
    pSesssion->send(res_stream.Data(), res_stream.Size());
    LogInfoA("[Net] Send STestStructResMsg");
  }
};

int main(void) {
  LOG_INIT("udp_server_example", true, "debug", true);

  uint32_t thread_num                            = std::thread::hardware_concurrency();
  thread_num                                     = 1;
  std::shared_ptr<ProcessorPool> processor_pool = std::make_shared<ProcessorPool>();
  processor_pool->start(thread_num);

  UDP_INIT();

  CSessionCreateFactory::Instance()->AddRelation<CTestMsgHandler, ICodecHandler>(0);

  uint32_t                  conv    = CKcpMgr::Instance()->GetConv();
  std::shared_ptr<CSession> session = CSessionCreateFactory::Instance()->CreateEmptySession();
  assert(session);
  session->set_conv_id(conv);
  session->start_kcp();
  CKcpMgr::Instance()->AddPrepare(conv, session);

  const char*                 local_ip   = "127.0.0.1";
  uint16_t                    local_port = 7001;
  std::shared_ptr<CUdpServer> udp_server = std::make_shared<CUdpServer>(processor_pool->get_processor()->get_io_context());
  udp_server->Start(local_ip, local_port);
  timer_print_qps(60);
  bool busy = false;
  while (true) {
    busy = false;
    if (CMainModule::Instance()->update()) {
      busy = true;
    }

    if (udp_server->Update()) {
      busy = true;
    }

    if (!busy) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }

  processor_pool->stop();

  UDP_RELEASE();
  LOG_RELEASE();

  return 0;
}