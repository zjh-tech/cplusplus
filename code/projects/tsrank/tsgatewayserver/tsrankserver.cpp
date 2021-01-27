/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-03 19:17:39
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-11-04 11:24:53
 */

#include "tsrankserver.h"
#include "engine/inc/log/env.h"

void TsRankServer::Init() {
}

void TsRankServer::OnEstablish(shared_ptr<SSServerSession> sess) {
}

void TsRankServer::OnTerminate(shared_ptr<SSServerSession> sess) {
}

void TsRankServer::OnHandlerMsg(shared_ptr<SSServerSession> sess, uint32_t msg_id, const char* attach, uint32_t attach_len, const char* msg, uint32_t msg_len) {
  auto dealer = iddealer.Find(msg_id);
  if (dealer == nullptr) {
    LogWarnA("TsRankServer OnHandlerMsg Can Not Find MsgID = {}", msg_id);
    return;
  }

  dealer(sess, msg_id, msg, msg_len);
}