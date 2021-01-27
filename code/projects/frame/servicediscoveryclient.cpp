/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-11 18:00:31
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-01-21 10:41:59
 */

#include "servicediscoveryclient.h"
#include "engine/inc/common/utils.hpp"
#include "engine/inc/log/env.h"
#include "frame/coder.h"
#include "pb/ss_base.pb.h"
#include "sdserversession.h"
#include "ssclientsession.h"

ServiceDiscoveryClient::ServiceDiscoveryClient() {
  timer_register       = make_shared<TimerRegister>();
  ssclient_session_mgr = make_shared<SSClientSessionMgr>();
}

bool ServiceDiscoveryClient::Init(const string& host, uint32_t port, uint64_t server_id, const string& token, SDClientCbFunc cb_func) {
  ssclient_session_mgr->Init();

  this->host      = host;
  this->port      = port;
  this->server_id = server_id;
  this->token     = token;
  this->cb_func   = cb_func;

  LogInfoA("[ServiceDiscoveryClient] Connect SessionID={} Host={} Port={}", session_id, host, port);
  session_id = ssclient_session_mgr->Connect(host, port, make_shared<SDServerSession>(), make_shared<Coder>());

  auto self = this->shared_from_this();
  timer_register->AddRepeatTimer(SD_CLIENT_RECONNECT_TIMER_ID, SD_CLIENT_RECONNECT_TIMER_DELAY, [this, self]() {
    if (sd_client_session_ptr != nullptr) {
      return;
    }

    if ((ssclient_session_mgr->IsInConnectCache(this->session_id) == false) && (ssclient_session_mgr->IsExistSessionOfSessID(this->session_id) == false)) {
      this->session_id = ssclient_session_mgr->Connect(this->host, this->port, make_shared<SDServerSession>(), make_shared<Coder>());
      LogInfoA("[ServiceDiscoveryClient] Reconnect SessionID={} Host={} Port={}", this->session_id, this->host, this->port);
    }
  });

  timer_register->AddRepeatTimer(SD_CLIENT_SEND_REQ_TIMER_ID, SD_CLIENT_SEND_REQ_TIMER_DELAY, [this, self]() {
    if (sd_client_session_ptr == nullptr) {
      return;
    }

    pb::service_discovery_req req;
    req.set_server_id(this->server_id);
    req.set_token(this->token);
    sd_client_session_ptr->SendProtoMsg(pb::service_discovery_req_id, req, nullptr);
    LogDebugA("[ServiceDiscoveryClient] Send ServiceDiscoveryReq ServerID={}", this->server_id);
  });
  return true;
}

void ServiceDiscoveryClient::SetSDClientSession(shared_ptr<SSClientSession> sess) {
  sd_client_session_ptr = sess;
}

shared_ptr<ServiceDiscoveryClient> GServiceDiscoveryClient = nullptr;