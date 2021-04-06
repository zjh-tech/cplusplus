#include "dbsession.h"

void DBSession::Init()
{
}

void DBSession::OnEstablish(shared_ptr<SSServerSession> sess)
{
}

void DBSession::OnTerminate(shared_ptr<SSServerSession> sess)
{
}

void DBSession::OnHandlerMsg(shared_ptr<SSServerSession> sess, uint32_t msg_id, const char* attach, uint32_t attach_len, const char* msg, uint32_t msg_len)
{
}