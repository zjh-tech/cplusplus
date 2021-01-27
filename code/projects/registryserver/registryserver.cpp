/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-04 15:28:26
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-01-08 16:34:50
 */

#include "registryserver.h"
#include "engine/inc/log/env.h"
#include "engine/inc/tcp/env.h"
#include "engine/inc/timer/env.h"
#include "frame/coder.h"
#include "frame/servercfg.h"
#include "frame/ssclientsession.h"
#include "registrycfg.h"
#include "sdclientsession.h"
#include "servicediscoveryserver.h"
#include <thread>

using namespace std;

bool RegistryServer::Init() {
  if (Server::Init() == false) {
    return false;
  }

  tuple<shared_ptr<RegistryCfg>, ErrorStringPtr> cfg_attr = LoadRegistryCfg("./service_registry.xml");
  if (get<1>(cfg_attr) != nullptr) {
    return false;
  }
  GRegistryCfg = get<0>(cfg_attr);

  GServiceDiscoveryServer->Init("./service_registry.xml");

  GSSClientSessionMgr->Listen(GServerCfg->SDListenIp, GServerCfg->SDListenPort, make_shared<SDClientSession>(), make_shared<Coder>(), GServer->GetIOContextPool());

  LogInfo("[RegistryServer] Init Ok");
  return true;
}

void RegistryServer::Run() {
  bool busy = false;

  auto net_module   = Net::Instance();
  auto timer_module = TimeWheelMgr::Instance();

  while (!IsQuit()) {
    busy = false;

    if (net_module->Run(NET_LOOP_COUNT)) {
      busy = true;
    }

    if (timer_module->Run(TIMER_LOOP_COUNT)) {
      busy = true;
    }

    if (!busy) {
      this_thread::sleep_for(chrono::milliseconds(1));
    }
  }
}