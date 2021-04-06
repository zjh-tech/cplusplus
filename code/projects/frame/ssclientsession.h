#pragma once

#include "session.h"

namespace Framework {
namespace Timer {
  class ITimerRegister;
}  // namespace Timer
}  // namespace Framework

using namespace std;
using namespace Framework::Timer;

class SSClientSession;

class ISSClientSessionHandler {
public:
  virtual ~ISSClientSessionHandler(){};

  virtual bool Init()                                                                                                                                   = 0;
  virtual void OnEstablish(shared_ptr<SSClientSession> sess)                                                                                            = 0;
  virtual void OnTerminate(shared_ptr<SSClientSession> sess)                                                                                            = 0;
  virtual void OnHandlerMsg(shared_ptr<SSClientSession> sess, uint32_t msg_id, const char* attach, uint32_t attach_len, const char* data, uint32_t len) = 0;
  virtual void OnBeatHeartError(shared_ptr<SSClientSession> sess)                                                                                       = 0;
};

class SSClientSession : public enable_shared_from_this<SSClientSession>, public CSession {
public:
  SSClientSession();
  virtual ~SSClientSession();

  void OnEstablish() override;
  void OnTerminate() override;
  void OnHandlerMsg(uint32_t msg_id, const char* attach, uint32_t attach_len, const char* msg, uint32_t msg_len);

  inline void SetHandler(shared_ptr<ISSClientSessionHandler> handler) {
    m_handler = handler;
  }

  inline void SendBytes(uint32_t msg_id, const char* msg, uint32_t len, IAttachParas* attach = nullptr) {
    AsyncSendBytes(msg_id, msg, len, attach);
  }

  inline void SendProtoMsg(uint32_t msg_id, const google::protobuf::Message& msg, IAttachParas* attach = nullptr) {
    AsyncSendProtoMsg(msg_id, msg, attach);
  }

private:
  shared_ptr<ISSClientSessionHandler> m_handler              = nullptr;
  shared_ptr<ITimerRegister>          m_timer_register       = nullptr;
  int64_t                             m_last_beat_heart_time = 0;

  enum eTimerID : uint32_t {
    BEAT_HEART_TIME_ID       = 1,
    SEND_BEAT_HEART_TIMER_ID = 2,
  };

  enum eTimerDelay : uint64_t {
    BEAT_HEART_TIME_DELAY       = 1000 * 60,
    SEND_BEAT_HEART_TIMER_DELAY = 1000 * 15,
  };

  const int64_t BEAT_HEART_MAX_TIME = 1000 * 60 * 5;
};

class SSClientSessionMgr : public enable_shared_from_this<SSClientSessionMgr>, public ISessionFactory {
public:
  SSClientSessionMgr();

  void Init();

  bool IsInConnectCache(uint64_t session_id);

  bool IsExistSessionOfSessID(uint64_t session_id);

  shared_ptr<ISession> CreateSession() override;

  void AddSession(shared_ptr<ISession> session_ptr) override;

  void RemoveSession(uint64_t session_id) override;

  uint64_t Connect(const string& host, uint32_t port, shared_ptr<ISSClientSessionHandler> handler, shared_ptr<ICoder> coder);

  void Listen(const string& ip, uint32_t port, shared_ptr<ISSClientSessionHandler> handler, shared_ptr<ICoder> coder, shared_ptr<IOContextPool> io_context_pool);

private:
  inline void setCoder(shared_ptr<ICoder> coder) {
    m_coder = coder;
  }

  inline void setHandler(shared_ptr<ISSClientSessionHandler> handler) {
    m_handler = handler;
  }

private:
  uint64_t                            m_session_id = 0;
  map<uint64_t, shared_ptr<ISession>> m_session_map;
  shared_ptr<ISSClientSessionHandler> m_handler = nullptr;
  shared_ptr<ICoder>                  m_coder   = nullptr;
  shared_ptr<Listener>                m_listen  = nullptr;

  struct ConnectCache {
    ConnectCache(uint64_t session_id, string host, uint32_t port, int64_t connect_tick) {
      SessionID   = session_id;
      Host        = host;
      Port        = port;
      ConnectTick = connect_tick;
    }
    uint64_t SessionID   = 0;
    string   Host        = "";
    uint32_t Port        = 0;
    int64_t  ConnectTick = 0;
  };

  map<uint64_t, shared_ptr<ConnectCache>> m_connectCache_map;
  shared_ptr<ITimerRegister>              m_timer_register = nullptr;

  enum eTimerID : uint32_t {
    MGR_CONNECT_TIMEOUT_TIMER_ID = 1,
  };

  enum eTimerDelay : uint64_t {
    MGR_CONNECT_TIMEOUT_TIMER_DELAY = 1000 * 60,
  };
  const int64_t MGR_BEAT_HEART_MAX_TIME = 1000 * 10;
};

extern shared_ptr<SSClientSessionMgr> GSSClientSessionMgr;
