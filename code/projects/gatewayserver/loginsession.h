#pragma once

#include "frame/ilogicserver.h"

class LoginSession : public ILogicServer {
public:
  void Init() override;
  void OnEstablish(shared_ptr<SSServerSession> sess) override;
  void OnTerminate(shared_ptr<SSServerSession> sess) override;
  void OnHandlerMsg(shared_ptr<SSServerSession> sess, uint32_t msg_id, const char* attach, uint32_t attach_len, const char* msg, uint32_t msg_len) override;
};