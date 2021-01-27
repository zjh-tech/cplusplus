/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-10-19 11:48:36
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-01-08 16:29:37
 */

#include "ssserverSession.h"
#include "engine/inc/common/convert.hpp"
#include "engine/inc/common/utils.hpp"
#include "engine/inc/log/env.h"
#include "engine/inc/tcp/listener.h"
#include "frame/coder.h"
#include "globalset.h"
#include "ilogicserver.h"
#include "pb/ss_base.pb.h"
#include "server.h"

SSServerSession::SSServerSession() {
  m_timer_register = make_shared<TimerRegister>();
}

void SSServerSession::OnEstablish() {
  m_session_state = eSessionState::VERIFY;
  auto factory    = GetSessionFactory();
  assert(factory);
  auto self = this->shared_from_this();
  factory->AddSession(self);
  m_last_beat_heart_time = Utils::GetMillTime();

  if (IsConnectSessionType()) {
    LogInfoA("[SSServerSession] SessionID={} ServerID={},ServerType={} Send Verify Req", GetSessionID(), m_remote_server_id, m_remote_server_type_str);

    pb::s2s_server_session_veriry_req req;
    req.set_server_id(GServer->GetServerId());
    req.set_server_type(GServer->GetServerType());
    req.set_server_type_str(GServer->GetServerName());
    req.set_token(m_remote_token);  //md5
    AsyncSendProtoMsg(pb::s2s_server_session_veriry_req_id, req);
    return;
  }
}

void SSServerSession::OnTerminate() {
  if (m_remote_server_id == 0) {
    LogInfoA("[SSServerSession] OnTerminate SessionID={}", GetSessionID());
  } else {
    LogInfoA("[SSServerSession] OnTerminate SessionID={} ServerID={},ServerType={}", GetSessionID(), m_remote_server_id, m_remote_server_type_str);
  }

  m_timer_register->KillAllTimer();
  auto factory = GetSessionFactory();
  assert(factory);
  factory->RemoveSession(GetSessionID());

  auto self = this->shared_from_this();
  if (m_session_state == eSessionState::ESTABLISH) {
    m_logic_server_ptr->SetSession(nullptr);
    m_logic_server_ptr->OnTerminate(self);
  }

  m_session_state = eSessionState::CLOSE;
}

void SSServerSession::OnHandlerMsg(uint32_t msg_id, const char* attach, uint32_t attach_len, const char* msg, uint32_t msg_len) {
  if (msg_id == pb::s2s_server_session_veriry_req_id && IsListenSessionType()) {
    pb::s2s_server_session_veriry_req req;
    req.ParseFromArray(msg, msg_len);

    auto ack_func = [this](uint32_t errror_code) {
      if (errror_code == eMsgCode::FAIL) {
        Terminate();
        return;
      }

      pb::s2s_server_session_veriry_ack ack;
      AsyncSendProtoMsg(pb::s2s_server_session_veriry_ack_id, ack);
    };

    auto factory = GetSessionFactory();
    if (factory == nullptr) {
      LogErrorA("[SSServerSession] SessionID={}  OnHandlerMsg SessionFactoryPtr Is Null", GetSessionID());
      return;
    }

    auto server_factory = dynamic_pointer_cast<SSServerSessionMgr>(factory);
    if (server_factory == nullptr) {
      LogErrorA("[SSServerSession] SessionID={}  OnHandlerMsg SSServerSessionFactoryPtr Is Null", GetSessionID());
      return;
    }

    if (server_factory->IsExistSessionOfSrvID(req.server_id())) {
      LogErrorA("[SSServerSession] SessionID={} ServerID={}  Already Exist", GetSessionID(), req.server_id());
      ack_func(eMsgCode::FAIL);
      return;
    }

    SetRemoteServerID(req.server_id());
    SetRemoteServerType(req.server_type());
    SetRemoteServerTypeStr(req.server_type_str());
    SetRemoteToken(req.token());

    //md5
    if (req.token() != GServer->GetToken()) {
      LogErrorA("[SSServerSession] SessionID={} ServerID={},ServerType={} Token Error Verify Fail", GetSessionID(), m_remote_server_id, m_remote_server_type_str);
      ack_func(eMsgCode::FAIL);
      return;
    }

    LogInfoA("[SSServerSession] SessionID={} ServerID={},ServerType={} Verify Ok", GetSessionID(), m_remote_server_id, m_remote_server_type_str);
    OnVerify();
    ack_func(eMsgCode::SUCCESS);
    return;
  }

  if (msg_id == pb::s2s_server_session_veriry_ack_id && IsConnectSessionType()) {
    LogInfoA("[SSServerSession] SessionID={} ServerID={},ServerType={} Verify Ok", GetSessionID(), m_remote_server_id, m_remote_server_type_str);
    OnVerify();
    return;
  }

  if (msg_id == pb::s2s_server_session_ping_id && IsListenSessionType()) {
    m_last_beat_heart_time = Utils::GetMillTime();
    pb::s2s_server_session_pong ack;
    AsyncSendProtoMsg(pb::s2s_server_session_pong_id, ack);
    LogDebugA("[SSServerSession] SessionID={} ServerID={},ServerType={} Recv Ping Send Pong", GetSessionID(), m_remote_server_id, m_remote_server_type_str);
    return;
  }

  if (msg_id == pb::s2s_server_session_pong_id && IsConnectSessionType()) {
    m_last_beat_heart_time = Utils::GetMillTime();
    LogDebugA("[SSServerSession] SessionID={} ServerID={},ServerType={} Recv Pong", GetSessionID(), m_remote_server_id, m_remote_server_type_str);
    return;
  }

  m_last_beat_heart_time = Utils::GetMillTime();
  auto self              = this->shared_from_this();
  m_logic_server_ptr->OnHandlerMsg(self, msg_id, attach, attach_len, msg, msg_len);
}

void SSServerSession::OnVerify() {
  m_session_state = eSessionState::ESTABLISH;
  LogInfoA("[SSServerSession] OnVerify SessionID={} ServerID={},ServerType={}", GetSessionID(), m_remote_server_id, m_remote_server_type_str);
  auto factory = GetSessionFactory();
  assert(factory);
  auto self           = this->shared_from_this();
  auto server_factory = dynamic_pointer_cast<SSServerSessionMgr>(factory);
  assert(server_factory);
  auto logicfactory = server_factory->GetLogicServerFactory();
  assert(logicfactory);
  logicfactory->SetLogicServer(self);
  m_logic_server_ptr->SetSession(self);
  m_logic_server_ptr->OnEstablish(self);

  m_timer_register->AddRepeatTimer(S2S_CHECK_BEAT_HEART_TIMER_ID, S2S_CHECK_BEAT_HEART_TIMER_DELAY, [this, self]() {
    int64_t now = Utils::GetMillTime();
    if ((m_last_beat_heart_time + S2S_BEAT_HEART_MAX_TIME) < now) {
      LogInfoA("[SSServerSession] SessionID={} ServerID={},ServerType={}  BeatHeart Exception", GetSessionID(), m_remote_server_id, m_remote_server_type_str);
      Terminate();
    }
  });

  if (IsConnectSessionType()) {
    m_timer_register->AddRepeatTimer(S2S_SEND_BEAT_HEART_TIMER_ID, S2S_SEND_BEAT_HEART_TIMER_DELAY, [this, self]() {
      pb::s2s_server_session_ping req;
      AsyncSendProtoMsg(pb::s2s_server_session_ping_id, req);
      LogDebugA("[SSServerSession] SessionID={} ServerID={},ServerType={}  Send Ping", GetSessionID(), m_remote_server_id, m_remote_server_type_str);
    });
  }
}

void SSServerSession::SetLogicServer(shared_ptr<ILogicServer> logic_server) {
  m_logic_server_ptr = logic_server;
  m_logic_server_ptr->Init();
}

SSServerSessionMgr::SSServerSessionMgr() {
  m_timer_register = make_shared<TimerRegister>();
}

void SSServerSessionMgr::Init(shared_ptr<ILogicServerFactory> factory) {
  m_logic_server_factory = factory;

  m_timer_register->AddRepeatTimer(S2SMGR_OUTPUT_TIMER_ID, S2SMGR_OUTPUT_TIMER_DELAY, [this]() {
    for (auto iter = m_session_map.begin(); iter != m_session_map.end(); ++iter) {
      auto session = dynamic_pointer_cast<SSServerSession>(iter->second);
      LogInfoA("[SSServerSessionMgr] OutPut ServerID={},ServerType={}", session->GetRemoteServerID(), session->GetRemoteServerTypeStr());
    }

    int64_t now = Utils::GetMillTime();
    for (auto iter = m_connectCache_map.begin(); iter != m_connectCache_map.end();) {
      if (iter->second->ConnectTick < now) {
        LogInfoA("[SSServerSessionMgr] Timeout Triggle ConnectCache Del SessionID={},ServerID={}", iter->second->SessionID, iter->second->ServerID);
        iter = m_connectCache_map.erase(iter);
      } else {
        ++iter;
      }
    }
  });
}

bool SSServerSessionMgr::IsInConnectCache(uint64_t server_id) {
  auto iter = m_connectCache_map.find(server_id);
  return (iter != m_connectCache_map.end()) ? true : false;
}

shared_ptr<ISession> SSServerSessionMgr::CreateSession() {
  ++m_session_id;
  shared_ptr<ISession> sessionPtr = make_shared<SSServerSession>();
  assert(sessionPtr);
  sessionPtr->SetSessionID(m_session_id);

  shared_ptr<ICoder> coder = make_shared<Coder>();
  sessionPtr->SetCoder(coder);
  auto self = this->shared_from_this();
  sessionPtr->SetSessionFactory(self);
  LogInfoA("[SSServerSessionMgr] CreateSession SessionID={}", m_session_id);

  return sessionPtr;
}

void SSServerSessionMgr::AddSession(shared_ptr<ISession> session_ptr) {
  uint64_t session_id       = session_ptr->GetSessionID();
  m_session_map[session_id] = session_ptr;
  LogInfoA("[SSServerSessionMgr] AddSession SessionID={}", session_id);

  auto SSServerSession_ptr = dynamic_pointer_cast<SSServerSession>(session_ptr);
  assert(SSServerSession_ptr);
  auto iter = m_connectCache_map.find(SSServerSession_ptr->GetRemoteServerID());
  if (iter != m_connectCache_map.end()) {
    LogInfoA("[SSServerSessionMgr] AddSession Triggle ConnectCache Del SessionID={},ServerID={}", session_id, iter->second->ServerID);
    m_connectCache_map.erase(iter);
  }
}

void SSServerSessionMgr::RemoveSession(uint64_t session_id) {
  auto iter = m_session_map.find(session_id);
  if (iter != m_session_map.end()) {
    LogInfoA("[SSServerSessionMgr] RemoveSession SessionID={}", m_session_id);
    m_session_map.erase(iter);
  }
}

bool SSServerSessionMgr::IsExistSessionOfSrvID(uint64_t server_id) {
  for (auto iter = m_session_map.begin(); iter != m_session_map.end(); ++iter) {
    auto session = dynamic_pointer_cast<SSServerSession>(iter->second);
    if (session->GetRemoteServerID() == server_id) {
      return true;
    }
  }

  return false;
}

shared_ptr<ISession> SSServerSessionMgr::FindSessionByServerID(uint64_t server_id) {
  for (auto iter = m_session_map.begin(); iter != m_session_map.end(); ++iter) {
    auto session = dynamic_pointer_cast<SSServerSession>(iter->second);
    if (session->GetRemoteServerID() == server_id) {
      return iter->second;
    }
  }

  return nullptr;
}

shared_ptr<ISession> SSServerSessionMgr::FindSessionBySessionID(uint64_t session_id) {
  auto iter = m_session_map.find(session_id);
  return (iter != m_session_map.end()) ? iter->second : nullptr;
}

vector<shared_ptr<ILogicServer>> SSServerSessionMgr::GetLogicSrvVecBySrvType(uint32_t server_type) {
  vector<shared_ptr<ILogicServer>> vec;
  for (auto iter = m_session_map.begin(); iter != m_session_map.end(); ++iter) {
    auto session = dynamic_pointer_cast<SSServerSession>(iter->second);
    if (session->GetRemoteServerType() == server_type) {
      vec.push_back(session->GetLogicServer());
    }
  }

  return vec;
}

uint64_t SSServerSessionMgr::GetSessIDBySrvTypeAndHashId(uint32_t server_type, uint64_t hash_id) {
  auto logic_srv_vec = GetLogicSrvVecBySrvType(server_type);
  if (logic_srv_vec.size() == 0) {
    return 0;
  }

  return logic_srv_vec[hash_id % logic_srv_vec.size()]->GetSession()->GetSessionID();
}

bool SSServerSessionMgr::SendProtoMsgBySrvId(uint64_t server_id, uint32_t msg_id, const google::protobuf::Message& msg, IAttachParas* attach) {
  auto isession = FindSessionByServerID(server_id);
  if (isession == nullptr) {
    return false;
  }
  auto session = dynamic_pointer_cast<SSServerSession>(isession);
  assert(session);
  session->AsyncSendProtoMsg(msg_id, msg, attach);
  return true;
}

bool SSServerSessionMgr::SendProtoMsgBySessId(uint64_t session_id, uint32_t msg_id, const google::protobuf::Message& msg, IAttachParas* attach) {
  auto isession = FindSessionBySessionID(session_id);
  if (isession == nullptr) {
    return false;
  }

  auto session = dynamic_pointer_cast<SSServerSession>(isession);
  assert(session);
  session->AsyncSendProtoMsg(msg_id, msg, attach);
  return true;
}

bool SSServerSessionMgr::SendBytesMsgBySrvId(uint64_t server_id, uint32_t msg_id, const char* msg, uint32_t len, IAttachParas* attach) {
  auto isession = FindSessionByServerID(server_id);
  if (isession == nullptr) {
    return false;
  }
  auto session = dynamic_pointer_cast<SSServerSession>(isession);
  assert(session);
  session->AsyncSendBytes(msg_id, msg, len, attach);
  return true;
}

bool SSServerSessionMgr::SendBytesMsgBySessId(uint64_t session_id, uint32_t msg_id, const char* msg, uint32_t len, IAttachParas* attach) {
  auto isession = FindSessionBySessionID(session_id);
  if (isession == nullptr) {
    return false;
  }

  auto session = dynamic_pointer_cast<SSServerSession>(isession);
  assert(session);
  session->AsyncSendBytes(msg_id, msg, len, attach);
  return true;
}

void SSServerSessionMgr::BroadCastBytesMsgBySrvType(uint32_t server_type, uint32_t msg_id, const char* msg, uint32_t len, IAttachParas* attach) {
  for (auto iter = m_session_map.begin(); iter != m_session_map.end(); ++iter) {
    auto session = dynamic_pointer_cast<SSServerSession>(iter->second);
    if (session->GetRemoteServerType() == server_type) {
      auto isession = iter->second;
      assert(isession);
      auto session = dynamic_pointer_cast<SSServerSession>(isession);
      if (session) {
        session->AsyncSendBytes(msg_id, msg, len, attach);
      }
    }
  }
}

void SSServerSessionMgr::BroadCastProtoMsgBySrvType(uint32_t server_type, uint32_t msg_id, const google::protobuf::Message& msg, IAttachParas* attach) {
  for (auto iter = m_session_map.begin(); iter != m_session_map.end(); ++iter) {
    auto session = dynamic_pointer_cast<SSServerSession>(iter->second);
    if (session->GetRemoteServerType() == server_type) {
      auto isession = iter->second;
      assert(isession);
      auto session = dynamic_pointer_cast<SSServerSession>(isession);
      if (session) {
        session->AsyncSendProtoMsg(msg_id, msg, attach);
      }
    }
  }
}

bool SSServerSessionMgr::SendProtoMsgByHashIdAndSrvType(uint32_t server_type, uint64_t hash_id, uint32_t msg_id, const google::protobuf::Message& msg, IAttachParas* attach) {
  auto logic_srv_vec = GetLogicSrvVecBySrvType(server_type);
  if (logic_srv_vec.size() == 0) {
    return false;
  }

  logic_srv_vec[hash_id % logic_srv_vec.size()]->GetSession()->AsyncSendProtoMsg(msg_id, msg, attach);
  return true;
}

void SSServerSessionMgr::Connect(uint64_t remote_server_id, uint32_t remote_server_type, const string& remote_server_type_str, const string& remote_ip, uint32_t remote_port, const string& remote_token) {
  auto isession = CreateSession();
  assert(isession);
  auto session = dynamic_pointer_cast<SSServerSession>(isession);
  assert(session);
  session->SetRemoteServerID(remote_server_id);
  session->SetRemoteServerType(remote_server_type);
  session->SetRemoteServerTypeStr(remote_server_type_str);
  session->SetRemoteToken(remote_token);
  session->SetConnectSessionType();

  int64_t tick                         = Utils::GetMillTime() + S2SMGR_BEAT_HEART_MAX_TIME;
  auto    cache_ptr                    = make_shared<ConnectCache>(session->GetSessionID(), remote_server_id, remote_server_type, remote_server_type_str, tick);
  m_connectCache_map[remote_server_id] = cache_ptr;
  LogInfoA("[SSServerSessionMgr] ConnectCache Add SessionID={},ServerID={}", m_session_id, remote_server_id);

  Net::Instance()->DoConnect(GServer->GetNextIoContext(), remote_ip, remote_port, isession);
}

void SSServerSessionMgr::Listen(const string& ip, uint32_t port, shared_ptr<IOContextPool> io_context_pool, ListenCbFunc fail_cb_func) {
  m_listen  = make_shared<Listener>(io_context_pool);
  auto self = this->shared_from_this();
  m_listen->start(ip, port, self, fail_cb_func);
}

shared_ptr<SSServerSessionMgr> GSSServerSessionMgr = make_shared<SSServerSessionMgr>();