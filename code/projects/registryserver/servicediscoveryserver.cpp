/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-11 10:47:23
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-12-29 17:05:32
 */

#include "servicediscoveryserver.h"
#include "engine/inc/common/utils.hpp"
#include "engine/inc/log/env.h"
#include "registrycfg.h"

shared_ptr<ServiceDiscoveryServer> GServiceDiscoveryServer = make_shared<ServiceDiscoveryServer>();

ServiceDiscoveryServer::ServiceDiscoveryServer() {
  timer_register = make_shared<TimerRegister>();
}

void ServiceDiscoveryServer::Init(string path) {
  auto self = this->shared_from_this();
  timer_register->AddOnceTimer(REBUILD_SERVICE_LIST_TIMER_ID, REBUILD_SERVICE_LIST_TIMER_DELAY, [this, self]() {
    rebuild_flag = false;
    LogInfoA("[ServiceDiscovery]  Rebuild  ServiceList OK");
  });

  timer_register->AddRepeatTimer(RELOAD_REGISTRY_CFG_TIMER_ID, RELOAD_REGISTRY_CFG_TIMER_DELAY, [this, self, path]() {
    tuple<shared_ptr<RegistryCfg>, ErrorStringPtr> cfg_attr = LoadRegistryCfg(path.c_str());
    if (get<1>(cfg_attr) != nullptr) {
      return;
    }
    GRegistryCfg = get<0>(cfg_attr);
    LogInfoA("[ServiceDiscovery]  Reload Registry Cfg");
  });

  timer_register->AddRepeatTimer(PRINT_USED_SERVICE_TIMER_ID, PRINT_USED_SERVICE_TIMER_DELAY, [this, self]() {
    for (auto iter = used_service_map.begin(); iter != used_service_map.end(); ++iter) {
      LogInfoA("[ServiceDiscovery]  Used Service ServerID={}", iter->first);
    }
  });

  timer_register->AddRepeatTimer(WARN_SERVICE_TIMER_ID, WARN_SERVICE_TIMER_DELAY, [this, self]() {
    if (rebuild_flag) {
      return;
    }

    int64_t now = Utils::GetMillTime();
    for (auto iter = used_service_map.begin(); iter != used_service_map.end();) {
      if (iter->second->WarnFlag == false && iter->second->Tick + SERVICE_WARN_TIME < now) {
        iter->second->WarnFlag = true;
        LogInfoA("[ServiceDiscovery] Add Warn Service ServerID={}", iter->second->ServerID);
      }

      if (iter->second->WarnFlag == true && iter->second->Tick + SERVICE_MAX_TIMEOUT_TIME < now) {
        LogInfoA("[ServiceDiscovery] Del Used Service ServerID={}", iter->second->ServerID);
        iter = used_service_map.erase(iter);
      } else {
        ++iter;
      }
    }
  });
}

void ServiceDiscoveryServer::AddUsedService(uint64_t server_id) {
  auto service_ptr      = make_shared<ServiceDiscoveryData>();
  service_ptr->ServerID = server_id;
  service_ptr->Tick     = Utils::GetMillTime();
  service_ptr->WarnFlag = false;

  used_service_map[server_id] = service_ptr;
  LogInfoA("[ServiceDiscovery] AddUsedService ServerID={}", server_id);
}

void ServiceDiscoveryServer::RemoveWarnService(uint64_t server_id) {
  auto iter = used_service_map.find(server_id);
  if (iter != used_service_map.end()) {
    iter->second->Tick = Utils::GetMillTime();
    if (iter->second->WarnFlag) {
      iter->second->WarnFlag = false;
      LogInfoA("[ServiceDiscovery] RemoveWarnService ServerID={}", server_id);
    }
  }
}