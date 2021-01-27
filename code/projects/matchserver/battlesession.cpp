/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-17 10:43:16
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-11-17 18:23:13
 */

#include "battlesession.h"

void BattleSession::Init() {
}

void BattleSession::OnEstablish(shared_ptr<SSServerSession> sess) {
}

void BattleSession::OnTerminate(shared_ptr<SSServerSession> sess) {
}

void BattleSession::OnHandlerMsg(shared_ptr<SSServerSession> sess, uint32_t msg_id, const char* attach, uint32_t attach_len, const char* msg, uint32_t msg_len) {
}