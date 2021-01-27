/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-17 10:43:08
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-11-17 11:25:58
 */

#pragma once

#include "frame/ilogicserver.h"

class GatewaySession : public ILogicServer {
public:
  void Init() override;
  void OnEstablish(shared_ptr<SSServerSession> sess) override;
  void OnTerminate(shared_ptr<SSServerSession> sess) override;
  void OnHandlerMsg(shared_ptr<SSServerSession> sess, uint32_t msg_id, const char* attach, uint32_t attach_len, const char* msg, uint32_t msg_len) override;
};