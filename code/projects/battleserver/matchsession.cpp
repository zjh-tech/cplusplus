/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-17 10:43:16
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-11-17 11:16:56
 */

#include "matchsession.h"

void MatchSession::Init() {
}

void MatchSession::OnEstablish(shared_ptr<SSServerSession> sess) {
}

void MatchSession::OnTerminate(shared_ptr<SSServerSession> sess) {
}

void MatchSession::OnHandlerMsg(shared_ptr<SSServerSession> sess, uint32_t msg_id, const char* attach, uint32_t attach_len, const char* msg, uint32_t msg_len) {
}