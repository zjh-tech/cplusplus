
#include "centersession.h"

void CenterSession::Init() {
}

void CenterSession::OnEstablish(shared_ptr<SSServerSession> sess) {
}

void CenterSession::OnTerminate(shared_ptr<SSServerSession> sess) {
}

void CenterSession::OnHandlerMsg(shared_ptr<SSServerSession> sess, uint32_t msg_id, const char* attach, uint32_t attach_len, const char* msg, uint32_t msg_len) {
}