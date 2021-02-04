/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-04 15:28:26
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-02-03 15:41:57
 */

#include "dbserver.h"
#include "engine/inc/log/env.h"
#include "engine/inc/mysql/env.h"
#include "frame/csclientsession.h"
#include "frame/databasecfgmgr.h"
#include "frame/httpproducer.h"
#include "frame/server.h"
#include "frame/servercfg.h"
#include "frame/servicediscoveryclient.h"
#include "frame/servicediscoveryhttpclient.h"
#include "frame/ssserversession.h"
#include "servermgr.h"
#include <thread>

using namespace std;

bool DBServer::Init() {
  if (Server::Init() == false) {
    return false;
  }

  DB_INIT();

  auto error_ptr = GDatabaseCfgMgr->Load("./db_cfg.xml");
  if (error_ptr != nullptr) {
    LogError("[DBServer] Load DatabaseCfg Error={}", error_ptr->Str());
    return false;
  }

  if (GMysqlModule->Init(GDatabaseCfgMgr->ConnSpecs, GDatabaseCfgMgr->TableMaxCount, GDatabaseCfgMgr->ConnMaxCount, GServer->GetIOContextPool()) == false) {
    LogError("[DBServer] MysqlModule Error");
    return false;
  }

  GSSServerSessionMgr->Init(make_shared<ServerMgr>());

  //GServiceDiscoveryClient->Init(GServerCfg->SDConnectIp, GServerCfg->SDConnectPort, GServerCfg->ServerId, GServerCfg->Token, nullptr);
  GServiceDiscoveryHttpClient->Init(GServerCfg->SDClientUrl, GServerCfg->ServerId, GServerCfg->Token, nullptr);

  LogInfo("[DBServer] Init Ok");
  return true;
}

void DBServer::Run() {
  bool busy = false;

  auto net_module         = Net::Instance();
  auto timer_module       = TimeWheelMgr::Instance();
  auto http_client_module = HttpProducer::Instance();
  auto db_module          = GMysqlModule;

  while (!IsQuit()) {
    busy = false;

    if (net_module->Run(NET_LOOP_COUNT)) {
      busy = true;
    }

    if (timer_module->Run(TIMER_LOOP_COUNT)) {
      busy = true;
    }

    if (db_module->Run(DB_LOOP_COUNT)) {
      busy = true;
    }

    if (http_client_module->Run(HTTP_CLIENT_LOOP_COUNT)) {
      busy = true;
    }

    if (!busy) {
      this_thread::sleep_for(chrono::milliseconds(1));
    }
  }
}

void DBServer::UnInit() {
  DB_UNINIT();
  Server::UnInit();
}