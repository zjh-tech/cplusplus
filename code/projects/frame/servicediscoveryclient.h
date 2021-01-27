/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-11 18:00:02
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-01-21 10:46:19
 */

#pragma once
#include "engine/inc/common/singleton.hpp"
#include "engine/inc/timer/env.h"
#include <cinttypes>

class SSClientSession;
class SSClientSessionMgr;

typedef void (*SDClientCbFunc)();

class ServiceDiscoveryClient : public enable_shared_from_this<ServiceDiscoveryClient> {
public:
  ServiceDiscoveryClient();

  bool Init(const string& host, uint32_t port, uint64_t server_id, const string& token, SDClientCbFunc cb_func);

  void SetSDClientSession(shared_ptr<SSClientSession> sess);

  inline bool GetInitFlag() {
    return init_flag;
  }

  inline void SetInitFlag() {
    init_flag = true;
  }

  inline SDClientCbFunc GetCbFunc() {
    return cb_func;
  }

private:
  string         host      = "";
  uint32_t       port      = 0;
  uint64_t       server_id = 0;
  string         token     = "";
  SDClientCbFunc cb_func   = nullptr;

  bool                           init_flag             = false;
  shared_ptr<ITimerRegister>     timer_register        = nullptr;
  shared_ptr<SSClientSessionMgr> ssclient_session_mgr  = nullptr;
  shared_ptr<SSClientSession>    sd_client_session_ptr = nullptr;
  uint64_t                       session_id            = 0;

  enum eTimerID : uint32_t {
    SD_CLIENT_SEND_REQ_TIMER_ID  = 1,
    SD_CLIENT_RECONNECT_TIMER_ID = 2,
  };

  enum eTimerDelay : uint64_t {
    SD_CLIENT_SEND_REQ_TIMER_DELAY  = 1000 * 3,
    SD_CLIENT_RECONNECT_TIMER_DELAY = 1000 * 10,
  };
};

extern shared_ptr<ServiceDiscoveryClient> GServiceDiscoveryClient;

#define SD_INIT() \
  GServiceDiscoveryClient = make_shared<ServiceDiscoveryClient>();
