/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-10-19 11:45:33
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-01-08 16:29:48
 */

#include "ssclientsession.h"
#include "engine/inc/common/utils.hpp"
#include "engine/inc/log/env.h"
#include "engine/inc/tcp/listener.h"
#include "engine/inc/timer/env.h"
#include "frame/coder.h"
#include "pb/ss_base.pb.h"
#include "server.h"

SSClientSession::SSClientSession() {
  m_timer_register       = make_shared<TimerRegister>();
  m_last_beat_heart_time = Utils::GetMillTime();
}

SSClientSession::~SSClientSession() {
  if (m_timer_register == nullptr) {
    m_timer_register->KillAllTimer();
  }
}

void SSClientSession::OnEstablish() {
  auto self    = this->shared_from_this();
  auto factory = GetSessionFactory();
  assert(factory);
  factory->AddSession(self);
  LogInfoA("[SSClientSession] OnEstablish SessionID={} ", GetSessionID());
  m_handler->OnEstablish(self);
  m_last_beat_heart_time = Utils::GetMillTime();

  m_timer_register->AddRepeatTimer(BEAT_HEART_TIME_ID, BEAT_HEART_TIME_DELAY, [this, self]() {
    int64_t now = Utils::GetMillTime();
    if ((m_last_beat_heart_time + BEAT_HEART_MAX_TIME) < now) {
      LogErrorA("[SSClientSession] SessionID={} BeatHeart Exception", GetSessionID());
      m_handler->OnBeatHeartError(self);
      Terminate();
    }
  });

  if (IsConnectSessionType()) {
    m_timer_register->AddRepeatTimer(SEND_BEAT_HEART_TIMER_ID, SEND_BEAT_HEART_TIMER_DELAY, [this, self]() {
      pb::s2s_client_session_ping req;
      AsyncSendProtoMsg(pb::s2s_client_session_ping_id, req);
      LogDebugA("[SSClientSession] SessionID={} Send Ping", GetSessionID());
    });
  }
}

void SSClientSession::OnTerminate() {
  auto factory = GetSessionFactory();
  assert(factory);
  factory->RemoveSession(GetSessionID());

  m_timer_register->KillAllTimer();
  LogInfoA("[SSClientSession] OnTerminate SessionID={} ", GetSessionID());

  auto self = this->shared_from_this();
  m_handler->OnTerminate(self);
}

void SSClientSession::OnHandlerMsg(uint32_t msg_id, const char* attach, uint32_t attach_len, const char* msg, uint32_t msg_len) {
  if (msg_id == (uint32_t)pb::s2s_client_session_ping_id) {
    m_last_beat_heart_time = Utils::GetMillTime();
    LogDebugA("[SSClientSession] SessionID={} Recv Ping Send Pong", GetSessionID());
    pb::s2s_client_session_pong pong;
    AsyncSendProtoMsg(pb::s2s_client_session_pong_id, pong);
    return;
  } else if (msg_id == (uint32_t)pb::s2s_client_session_pong_id) {
    m_last_beat_heart_time = Utils::GetMillTime();
    LogDebugA("[SSClientSession] SessionID={} Recv Pong", GetSessionID());
    return;
  }

  auto self = this->shared_from_this();
  m_handler->OnHandlerMsg(self, msg_id, attach, attach_len, msg, msg_len);
  m_last_beat_heart_time = Utils::GetMillTime();
}

SSClientSessionMgr::SSClientSessionMgr() {
  m_timer_register = make_shared<TimerRegister>();
}

void SSClientSessionMgr::Init() {
  m_timer_register->AddRepeatTimer(MGR_CONNECT_TIMEOUT_TIMER_ID, MGR_CONNECT_TIMEOUT_TIMER_DELAY, [this]() {
    int64_t now = Utils::GetMillTime();
    for (auto iter = m_connectCache_map.begin(); iter != m_connectCache_map.end();) {
      if (iter->second->ConnectTick < now) {
        LogInfoA("[SSClientSessionMgr] Timeout Triggle ConnectCache Del SessionID={},Host={} Port={}", iter->second->SessionID, iter->second->Host, iter->second->Port);
        iter = m_connectCache_map.erase(iter);
      } else {
        ++iter;
      }
    }
  });
}

bool SSClientSessionMgr::IsInConnectCache(uint64_t session_id) {
  auto iter = m_connectCache_map.find(session_id);
  return (iter != m_connectCache_map.end()) ? true : false;
}

bool SSClientSessionMgr::IsExistSessionOfSessID(uint64_t session_id) {
  auto iter = m_session_map.find(session_id);
  return (iter != m_session_map.end()) ? true : false;
}

shared_ptr<ISession> SSClientSessionMgr::CreateSession() {
  ++m_session_id;
  shared_ptr<SSClientSession> sessionPtr = make_shared<SSClientSession>();
  assert(sessionPtr);
  sessionPtr->SetSessionID(m_session_id);
  sessionPtr->SetCoder(m_coder);
  sessionPtr->SetHandler(m_handler);
  auto self = this->shared_from_this();
  sessionPtr->SetSessionFactory(self);
  LogInfoA("[SSClientSessionMgr] CreateSession SessionID={}", m_session_id);

  return sessionPtr;
}

void SSClientSessionMgr::AddSession(shared_ptr<ISession> session_ptr) {
  m_session_map[session_ptr->GetSessionID()] = session_ptr;

  auto iter = m_connectCache_map.find(session_ptr->GetSessionID());
  if (iter != m_connectCache_map.end()) {
    m_connectCache_map.erase(iter);
    LogInfoA("[SSClientSessionMgr] AddSession Triggle ConnectCache Del SessionID={}", session_ptr->GetSessionID());
  }
}

void SSClientSessionMgr::RemoveSession(uint64_t session_id) {
  auto iter = m_session_map.find(session_id);
  if (iter != m_session_map.end()) {
    LogInfoA("[SSClientSessionMgr] RemoveSession SessionID={}", m_session_id);
    m_session_map.erase(iter);
  }
}

uint64_t SSClientSessionMgr::Connect(const string& host, uint32_t port, shared_ptr<ISSClientSessionHandler> handler, shared_ptr<ICoder> coder) {
  handler->Init();

  auto isession = CreateSession();
  assert(isession);
  auto session = dynamic_pointer_cast<SSClientSession>(isession);
  assert(session);

  session->SetHandler(handler);
  session->SetCoder(coder);
  session->SetConnectSessionType();

  int64_t tick                     = Utils::GetMillTime() + MGR_BEAT_HEART_MAX_TIME;
  auto    cache_ptr                = make_shared<ConnectCache>(m_session_id, host, port, tick);
  m_connectCache_map[m_session_id] = cache_ptr;
  LogInfoA("[SSClientSessionMgr] ConnectCache Add SessionID={},Host={} Port={}", m_session_id, host, port);

  Net::Instance()->DoConnect(GServer->GetNextIoContext(), host, port, isession);
  return m_session_id;
}

void SSClientSessionMgr::Listen(const string& ip, uint32_t port, shared_ptr<ISSClientSessionHandler> handler, shared_ptr<ICoder> coder, shared_ptr<IOContextPool> io_context_pool) {
  assert(handler);
  assert(coder);

  handler->Init();

  setHandler(handler);
  setCoder(coder);

  m_listen  = make_shared<Listener>(io_context_pool);
  auto self = this->shared_from_this();
  m_listen->start(ip, port, self);
}

shared_ptr<SSClientSessionMgr> GSSClientSessionMgr = make_shared<SSClientSessionMgr>();