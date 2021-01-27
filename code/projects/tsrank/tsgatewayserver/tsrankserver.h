/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-03 19:16:59
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-11-04 12:06:46
 */

#pragma once

#include "common/msgidrange.h"
#include "frame/iddealer.hpp"
#include "frame/ilogicserver.h"

class TsRankServer;

typedef void (*TsRankServerFunc)(shared_ptr<SSServerSession> sess, uint32_t msg_id, const char* msg, uint32_t msg_len);

class TsRankServer : public ILogicServer {
public:
  void Init() override;

  void OnEstablish(shared_ptr<SSServerSession> sess) override;
  void OnTerminate(shared_ptr<SSServerSession> sess) override;
  void OnHandlerMsg(shared_ptr<SSServerSession> sess, uint32_t msg_id, const char* attach, uint32_t attach_len, const char* msg, uint32_t msg_len) override;

private:
  IDDealer<TsRankServerFunc, TS_GR_MIN_ID, TS_GR_MAX_ID> iddealer;
};