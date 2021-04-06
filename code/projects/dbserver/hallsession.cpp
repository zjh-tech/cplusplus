#include "hallsession.h"

void HallSession::Init() {
}

void HallSession::OnEstablish(shared_ptr<SSServerSession> sess) {
}

void HallSession::OnTerminate(shared_ptr<SSServerSession> sess) {
}

void HallSession::OnHandlerMsg(shared_ptr<SSServerSession> sess, uint32_t msg_id, const char* attach, uint32_t attach_len, const char* msg, uint32_t msg_len) {
}