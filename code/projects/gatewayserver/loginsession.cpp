#include "loginsession.h"

void LoginSession::Init()
{
}

void LoginSession::OnEstablish(shared_ptr<SSServerSession> sess)
{
}

void LoginSession::OnTerminate(shared_ptr<SSServerSession> sess)
{
}

void LoginSession::OnHandlerMsg(shared_ptr<SSServerSession> sess, uint32_t msg_id, const char* attach, uint32_t attach_len, const char* msg, uint32_t msg_len)
{
}