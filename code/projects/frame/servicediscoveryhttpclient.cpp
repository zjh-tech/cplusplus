/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2021-01-20 16:18:23
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-01-25 17:42:57
 */

#include "servicediscoveryhttpclient.h"
#include "engine/inc/common/utils.hpp"
#include "engine/inc/log/env.h"
#include "engine/inc/tcp/env.h"
#include "httpconsumer.h"
#include "pb/ss_base.pb.h"
#include "server.h"
#include "servercfg.h"
#include "ssserversession.h"

ServiceDiscoveryHttpClient::ServiceDiscoveryHttpClient() {
  timer_register = make_shared<TimerRegister>();
}

bool ServiceDiscoveryHttpClient::Init(const string& _url, uint64_t _server_id, const string& _token, SDClientHttpCbFunc _cb_func) {
  cb_func   = _cb_func;
  server_id = _server_id;
  token     = _token;
  all_url   = "http://" + _url;

  auto self = this->shared_from_this();
  timer_register->AddRepeatTimer(SERVICEDISCOVERY_HTTP_TIMER_ID, SERVICEDISCOVERY_HTTP_TIMER_DELAY, [this, self]() {
    shared_ptr<SDHttpClient> ptr = make_shared<SDHttpClient>(this->all_url, this->server_id, this->token);
    if (ptr != nullptr && ptr->Init()) {
      HttpConsumer::Instance()->Push(ptr);
      LogDebugA("[ServiceDiscoveryHttpClient] Send ServiceDiscoveryReq ServerID={}", this->server_id);
    }
  });

  return true;
}

SDHttpClient::SDHttpClient(string _all_url, uint64_t _server_id, string _token) {
  all_url   = _all_url;
  server_id = _server_id;
  token     = _token;
}

SDHttpClient::~SDHttpClient() {
}

void SDHttpClient::ExecHttpCmd() {
  pb::service_discovery_req req;
  req.set_server_id(server_id);
  req.set_token(token);

  char buffer[MAX_MESSAGE_SIZE] = {0};
  req.SerializeToArray(buffer, MAX_MESSAGE_SIZE);
  uint32_t len = req.GetCachedSize();

  NetStream stream;
  uint32_t  msg_id = (uint32_t)pb::service_discovery_req_id;
  stream << msg_id;
  stream.WriteBuff(buffer, len);
  DoHttpPost(all_url.c_str(), stream.Data(), stream.Size());
}

void SDHttpClient::OnExecuted() {
  NetStream* ss_ptr = GetResponseStream();
  if (ss_ptr->Size() <= sizeof(uint32_t)) {
    LogWarnA("SDHttpClient OnExecuted Len Warn");
    return;
  }

  uint32_t msg_id = 0;
  (*ss_ptr) >> msg_id;
  pb::service_discovery_ack ack;
  char*                     data_ptr  = const_cast<char*>(ss_ptr->Data()) + ss_ptr->GetRPos();
  size_t                    data_size = ss_ptr->Size() - ss_ptr->GetRPos();
  GSDHttpServerSession->OnHandlerMsg(msg_id, data_ptr, data_size);
}

bool SDHttpServerSession::Init() {
  return iddealer.Register(pb::service_discovery_ack_id, OnHandlerHttpServiceDiscoveryAck);
}

void SDHttpServerSession::OnHandlerMsg(uint32_t msg_id, const char* data, uint32_t len) {
  auto dealer = iddealer.Find(msg_id);
  if (dealer == nullptr) {
    LogWarnA("SDHttpServerSession OnHandlerMsg Can Not Find MsgID = {}", msg_id);
    return;
  }

  dealer(data, len);
}

bool OnHandlerHttpServiceDiscoveryAck(const char* msg, uint32_t msg_len) {
  pb::service_discovery_ack ack;
  PARSE_PROTO(ack, msg, msg_len);

  if (ack.rebuild_flag() == true) {
    LogInfoA("[ServiceDiscovery] Service List Rebuilding");
    return true;
  }

  if (ack.verify_flag() == false) {
    LogInfoA("[ServiceDiscovery] Token Verify Error");
    return true;
  }

  if (GServiceDiscoveryHttpClient->GetInitFlag() == false) {
    GServiceDiscoveryHttpClient->SetInitFlag();
    //S2S Listen
    if (ack.sd_info().s2s_inter_listen() != "" && ack.sd_info().s2s_outer_listen() != "") {
      tuple<bool, string, uint32_t> attr = ConvertHostAndPort(ack.sd_info().s2s_outer_listen());
      if (get<0>(attr) == false) {
        LogError("[ServiceDiscovery] S2S Outer Listen Error");
        GServer->Quit();
        return true;
      }

      GSSServerSessionMgr->Listen(get<1>(attr), get<2>(attr), GServer->GetIOContextPool(), []() {
        LogError("[ServiceDiscovery] S2S Listen Error");
        GServer->Quit();
      });
    }

    GServerCfg->C2SInterListen    = ack.sd_info().c2s_inter_listen();
    GServerCfg->C2SOuterListen    = ack.sd_info().c2s_outer_listen();
    GServerCfg->C2SListenMaxCount = ack.sd_info().c2s_max_count();

    //C2S Listen
    if (GServiceDiscoveryHttpClient->GetCbFunc() != nullptr) {
      //client coder is not same
      GServiceDiscoveryHttpClient->GetCbFunc()();
    }
  }

  //Add Connect
  uint32_t conn_size = ack.sd_info().conn_list_size();
  for (uint32_t i = 0; i < conn_size; ++i) {
    auto conn_attr = ack.sd_info().conn_list(i);
    if (conn_attr.server_id() == 0) {
      continue;
    }

    bool existflag = false;
    if (GSSServerSessionMgr->IsInConnectCache(conn_attr.server_id()) || GSSServerSessionMgr->IsExistSessionOfSrvID(conn_attr.server_id())) {
      existflag = true;
    }

    if (existflag == false) {
      tuple<bool, string, uint32_t> attr = ConvertHostAndPort(conn_attr.outer());
      if (get<0>(attr) == false) {
        LogInfoA("[ServiceDiscovery] S2S Outer Connnect Error");
        return true;
      }
      GSSServerSessionMgr->Connect(conn_attr.server_id(), conn_attr.server_type(), conn_attr.server_type_str(), get<1>(attr), get<2>(attr), conn_attr.token());
    }
  }

  return true;
}

shared_ptr<ServiceDiscoveryHttpClient> GServiceDiscoveryHttpClient = nullptr;
shared_ptr<SDHttpServerSession>        GSDHttpServerSession        = nullptr;