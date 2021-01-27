/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-11 10:47:17
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-11-17 17:52:18
 */

#pragma once
#include "engine/inc/common/singleton.hpp"
#include "engine/inc/timer/env.h"
#include <cinttypes>
#include <map>

using namespace std;

struct ServiceDiscoveryData {
  uint64_t ServerID = 0;
  int64_t  Tick     = 0;
  bool     WarnFlag = false;
};

class ServiceDiscoveryServer : public enable_shared_from_this<ServiceDiscoveryServer> {
public:
  ServiceDiscoveryServer();

  void Init(string path);

  void AddUsedService(uint64_t server_id);

  void RemoveWarnService(uint64_t server_id);

  inline bool GetRebuildFlag() {
    return rebuild_flag;
  }

public:
  shared_ptr<ITimerRegister>                      timer_register = nullptr;
  map<uint64_t, shared_ptr<ServiceDiscoveryData>> used_service_map;
  bool                                            rebuild_flag = true;

  enum eTimerID : uint32_t {
    RELOAD_REGISTRY_CFG_TIMER_ID  = 1,
    REBUILD_SERVICE_LIST_TIMER_ID = 2,
    WARN_SERVICE_TIMER_ID         = 3,
    PRINT_USED_SERVICE_TIMER_ID   = 4,
  };

  enum eTimerDelay : uint64_t {
    RELOAD_REGISTRY_CFG_TIMER_DELAY  = 1000 * 60 * 2,
    REBUILD_SERVICE_LIST_TIMER_DELAY = 1000 * 30,
    WARN_SERVICE_TIMER_DELAY         = 1000 * 1,
    PRINT_USED_SERVICE_TIMER_DELAY   = 1000 * 60,
  };

  const int64_t SERVICE_WARN_TIME        = 1000 * 10;
  const int64_t SERVICE_MAX_TIMEOUT_TIME = 1000 * 60 * 2;
};

extern shared_ptr<ServiceDiscoveryServer> GServiceDiscoveryServer;