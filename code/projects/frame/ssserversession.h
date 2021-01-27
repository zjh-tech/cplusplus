/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-10-19 11:48:09
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-12-24 15:41:31
 */
#pragma once

#include "engine/inc/timer/env.h"
#include "session.h"

using namespace std;

class ILogicServer;
class ILogicServerFactory;

class SSServerSession : public enable_shared_from_this<SSServerSession>, public CSession {
public:
  SSServerSession();

  void OnEstablish() override;
  void OnTerminate() override;
  void OnHandlerMsg(uint32_t msg_id, const char* attach, uint32_t attach_len, const char* msg, uint32_t msg_len) override;

  void OnVerify();

  inline uint64_t GetRemoteServerID() {
    return m_remote_server_id;
  }

  inline void SetRemoteServerID(uint64_t remote_server_id) {
    m_remote_server_id = remote_server_id;
  }

  inline uint32_t GetRemoteServerType() {
    return m_remote_server_type;
  }

  inline void SetRemoteServerType(uint32_t remote_server_type) {
    m_remote_server_type = remote_server_type;
  }

  inline const string& GetRemoteServerTypeStr() {
    return m_remote_server_type_str;
  }

  inline void SetRemoteServerTypeStr(const string& remote_server_type_str) {
    m_remote_server_type_str = remote_server_type_str;
  }

  inline shared_ptr<ILogicServer> GetLogicServer() {
    return m_logic_server_ptr;
  }

  void SetLogicServer(shared_ptr<ILogicServer> logic_server);

  inline void SetRemoteToken(const string& remote_token) {
    m_remote_token = remote_token;
  }

  inline void SendBytes(uint32_t msg_id, const char* msg, uint32_t len, IAttachParas* attach = nullptr) {
    AsyncSendBytes(msg_id, msg, len, attach);
  }

  inline void SendProtoMsg(uint32_t msg_id, const google::protobuf::Message& msg, IAttachParas* attach = nullptr) {
    AsyncSendProtoMsg(msg_id, msg, attach);
  }

private:
  shared_ptr<ITimerRegister> m_timer_register         = nullptr;
  uint64_t                   m_remote_server_id       = 0;
  uint32_t                   m_remote_server_type     = 0;
  string                     m_remote_server_type_str = "";  //can not use globalset.h GetServerName/GetTsServerName
  shared_ptr<ILogicServer>   m_logic_server_ptr       = nullptr;
  string                     m_remote_token;

  enum class eSessionState {
    VERIFY    = 1,
    ESTABLISH = 2,
    CLOSE     = 3,
  };

  eSessionState m_session_state        = eSessionState::VERIFY;
  int64_t       m_last_beat_heart_time = 0;

  enum eTimerID : uint32_t {
    S2S_CHECK_BEAT_HEART_TIMER_ID = 1,
    S2S_SEND_BEAT_HEART_TIMER_ID  = 2,
  };

  enum eTimerDelay : uint64_t {
    S2S_CHECK_BEAT_HEART_TIMER_DELAY = 1000 * 10,
    S2S_SEND_BEAT_HEART_TIMER_DELAY  = 1000 * 30,
  };

  const int64_t S2S_BEAT_HEART_MAX_TIME = 1000 * 60 * 3;
};

class SSServerSessionMgr : public enable_shared_from_this<SSServerSessionMgr>, public ISessionFactory {
public:
  SSServerSessionMgr();

  friend class SSServerSession;

public:
  void Init(shared_ptr<ILogicServerFactory> factory);

  bool IsInConnectCache(uint64_t server_id);

  bool IsExistSessionOfSrvID(uint64_t server_id);

  vector<shared_ptr<ILogicServer>> GetLogicSrvVecBySrvType(uint32_t server_type);

  uint64_t GetSessIDBySrvTypeAndHashId(uint32_t server_type, uint64_t hash_id);

public:
  bool SendProtoMsgBySrvId(uint64_t server_id, uint32_t msg_id, const google::protobuf::Message& msg, IAttachParas* attach = nullptr);

  bool SendProtoMsgBySessId(uint64_t session_id, uint32_t msg_id, const google::protobuf::Message& msg, IAttachParas* attach = nullptr);

  bool SendBytesMsgBySrvId(uint64_t server_id, uint32_t msg_id, const char* msg, uint32_t len, IAttachParas* attach = nullptr);

  bool SendBytesMsgBySessId(uint64_t session_id, uint32_t msg_id, const char* msg, uint32_t len, IAttachParas* attach = nullptr);

  void BroadCastBytesMsgBySrvType(uint32_t server_type, uint32_t msg_id, const char* msg, uint32_t len, IAttachParas* attach = nullptr);

  void BroadCastProtoMsgBySrvType(uint32_t server_type, uint32_t msg_id, const google::protobuf::Message& msg, IAttachParas* attach = nullptr);

  bool SendProtoMsgByHashIdAndSrvType(uint32_t server_type, uint64_t hash_id, uint32_t msg_id, const google::protobuf::Message& msg, IAttachParas* attach = nullptr);

public:
  void Connect(uint64_t remote_server_id, uint32_t remote_server_type, const string& remote_server_type_str, const string& remote_ip, uint32_t remote_port, const string& remote_token);

  void Listen(const string& ip, uint32_t port, shared_ptr<IOContextPool> io_context_pool, ListenCbFunc fail_cb_func);

private:
  shared_ptr<ISession> CreateSession() override;

  void AddSession(shared_ptr<ISession> session_ptr) override;

  void RemoveSession(uint64_t session_id) override;

  inline shared_ptr<ILogicServerFactory> GetLogicServerFactory() {
    return m_logic_server_factory;
  }

  shared_ptr<ISession> FindSessionByServerID(uint64_t server_id);

  shared_ptr<ISession> FindSessionBySessionID(uint64_t session_id);

private:
  uint64_t                            m_session_id = 0;
  map<uint64_t, shared_ptr<ISession>> m_session_map;
  shared_ptr<ILogicServerFactory>     m_logic_server_factory = nullptr;
  shared_ptr<ITimerRegister>          m_timer_register       = nullptr;
  shared_ptr<Listener>                m_listen               = nullptr;

  struct ConnectCache {
    ConnectCache(uint64_t session_id, uint64_t server_id, uint32_t server_type, string server_type_str, int64_t connect_tick) {
      SessionID     = session_id;
      ServerID      = server_id;
      ServerType    = server_type;
      ServerTypeStr = server_type_str;
      ConnectTick   = connect_tick;
    }
    uint64_t SessionID     = 0;
    uint64_t ServerID      = 0;
    uint32_t ServerType    = 0;
    string   ServerTypeStr = "";
    int64_t  ConnectTick   = 0;
  };

  map<uint64_t, shared_ptr<ConnectCache>> m_connectCache_map;

  enum eTimerID : uint32_t {
    S2SMGR_OUTPUT_TIMER_ID = 1,
  };

  enum eTimerDelay : uint64_t {
    S2SMGR_OUTPUT_TIMER_DELAY = 1000 * 60,
  };

  const int64_t S2SMGR_BEAT_HEART_MAX_TIME = 1000 * 10;
};

extern shared_ptr<SSServerSessionMgr> GSSServerSessionMgr;
