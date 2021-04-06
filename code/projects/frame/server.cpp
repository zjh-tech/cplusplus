#include "server.h"
#include "engine/inc/common/iocontextpool.h"
#include "engine/inc/log/env.h"
#include "engine/inc/timer/env.h"
#include "frame/servicediscoveryhttpclient.h"
#include "framecfgmgr.h"
#include "google/protobuf/message.h"
#include "httpconsumer.h"
#include "servercfg.h"
#include "signal.h"
#include <iostream>

using namespace std;

bool IServerFacade::RegisterSignalHandler() {
  if (SIG_ERR == signal(SIGINT, OnSignalQuit)) {
    return false;
  }

  if (SIG_ERR == signal(SIGTERM, OnSignalQuit)) {
    return false;
  }

  if (SIG_ERR == signal(SIGQUIT, OnSignalQuit)) {
    return false;
  }

  if (SIG_ERR == signal(SIGUSR1, OnSignalHandler)) {
    return false;
  }

  if (SIG_ERR == signal(SIGUSR2, OnSignalHandler)) {
    return false;
  }

  return true;
}

Server* GServer = nullptr;

void Server::Quit() {
  terminate = true;
  LogInfoA("Server Quit");
}

bool Server::Init() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  //Load ServerCfg
  ErrorStringPtr err = FrameCfgMgr::LoadServerCfg("server_cfg.xml");
  if (err != nullptr) {
    cout << err->Str() << endl;
    return false;
  }

  //Set Server Attr
  server_id   = GServerCfg->ServerId;
  server_type = GServerCfg->ServerType;
  token       = GServerCfg->Token;
  server_name = GServerCfg->ServerName;

  LOG_INIT(GServerCfg->LogDir, GServerCfg->ServerName, GServerCfg->LogLevel);

  io_context_pool_ptr = make_shared<IOContextPool>();
  assert(io_context_pool_ptr);

  if (GServerCfg->ThreadNum == 0) {
    GServerCfg->ThreadNum = thread::hardware_concurrency();
  }

  LogInfoA("[Server] Init ThreadNum = {}", GServerCfg->ThreadNum);
  io_context_pool_ptr->Start(GServerCfg->ThreadNum, NET_LOOP_COUNT);

  GServer = this;

  //signal
  RegisterSignalHandler();

  TIMER_INIT();

  TCP_INIT();

  SD_HTTP_INIT();

  return true;
}

void Server::UnInit() {
  io_context_pool_ptr->Stop();
  LogInfoA("[IOContextPool] Stop");

  TCP_UNINIT();

  TIMER_UNINIT();

  LOG_UNINIT();

  google::protobuf::ShutdownProtobufLibrary();
}