/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-11 15:07:17
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-01-21 10:26:44
 */

#include "sdserversession.h"
#include "csclientsession.h"
#include "engine/inc/log/env.h"
#include "frame/coder.h"
#include "pb/ss_base.pb.h"
#include "server.h"
#include "servercfg.h"
#include "servicediscoveryclient.h"
#include "ssserversession.h"

bool SDServerSession::Init() {
  return iddealer.Register(pb::service_discovery_ack_id, OnHandlerServiceDiscoveryAck);
}

void SDServerSession::OnEstablish(shared_ptr<SSClientSession> sess) {
  GServiceDiscoveryClient->SetSDClientSession(sess);
}

void SDServerSession::OnTerminate(shared_ptr<SSClientSession> sess) {
  GServiceDiscoveryClient->SetSDClientSession(nullptr);
}

void SDServerSession::OnHandlerMsg(shared_ptr<SSClientSession> sess, uint32_t msg_id, const char* attach, uint32_t attach_len, const char* data, uint32_t len) {
  auto dealer = iddealer.Find(msg_id);
  if (dealer == nullptr) {
    LogWarnA("SDServerSession OnHandlerMsg Can Not Find MsgID = {}", msg_id);
    return;
  }

  dealer(sess, data, len);
}

void SDServerSession::OnBeatHeartError(shared_ptr<SSClientSession> sess) {
}

bool OnHandlerServiceDiscoveryAck(shared_ptr<SSClientSession> sess, const char* msg, uint32_t msg_len) {
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

  if (GServiceDiscoveryClient->GetInitFlag() == false) {
    GServiceDiscoveryClient->SetInitFlag();
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
    if (GServiceDiscoveryClient->GetCbFunc() != nullptr) {
      //client coder is not same
      GServiceDiscoveryClient->GetCbFunc()();
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