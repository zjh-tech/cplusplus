#pragma once

#include "common/msgidrange.h"
#include "frame/csclientsession.h"
#include "frame/iddealer.hpp"

typedef bool (*ClientFunc)(shared_ptr<CSClientSession> sess, const char* msg, uint32_t msg_len);

class ClientSession : public ICSClientSessionHandler
{
public:
    bool Init();

    virtual void OnEstablish(shared_ptr<CSClientSession> sess) override;
    virtual void OnTerminate(shared_ptr<CSClientSession> sess) override;
    virtual void OnHandlerMsg(shared_ptr<CSClientSession> sess, uint32_t msg_id, const char* data, uint32_t len) override;
    virtual void OnBeatHeartError(shared_ptr<CSClientSession> sess) override;

private:
    IDDealer<ClientFunc, GATEWAY_CLIENT_MIN_ID, GATEWAY_CLIENT_MAX_ID> iddealer;
};
