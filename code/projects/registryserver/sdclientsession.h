#pragma once

#include "common/msgidrange.h"
#include "frame/iddealer.hpp"
#include "frame/ssclientsession.h"

typedef bool (*SDClientFunc)(shared_ptr<SSClientSession> sess, const char* msg, uint32_t msg_len);

class SDClientSession : public ISSClientSessionHandler
{
public:
    bool Init() override;

    void OnEstablish(shared_ptr<SSClientSession> sess) override;
    void OnTerminate(shared_ptr<SSClientSession> sess) override;
    void OnHandlerMsg(shared_ptr<SSClientSession> sess, uint32_t msg_id, const char* attach, uint32_t attach_len, const char* data, uint32_t len) override;
    void OnBeatHeartError(shared_ptr<SSClientSession> sess) override;

private:
    IDDealer<SDClientFunc, SD_MIN_ID, SD_MAX_ID> iddealer;
};

bool OnHandlerUdpServiceDiscoveryReq(shared_ptr<SSClientSession> sess, const char* msg, uint32_t msg_len);