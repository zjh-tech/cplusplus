
#include "gatewaysession.h"

void GatewaySession::Init()
{
}

void GatewaySession::OnEstablish(shared_ptr<SSServerSession> sess)
{
}

void GatewaySession::OnTerminate(shared_ptr<SSServerSession> sess)
{
}

void GatewaySession::OnHandlerMsg(shared_ptr<SSServerSession> sess, uint32_t msg_id, const char* attach, uint32_t attach_len, const char* msg, uint32_t msg_len)
{
}