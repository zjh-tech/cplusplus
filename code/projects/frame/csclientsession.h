#pragma once

#include "session.h"

namespace Framework {
namespace Timer {
  class ITimerRegister;
}  // namespace Timer
}  // namespace Framework

using namespace std;
using namespace Framework::Timer;

class CSClientSession;

class ICSClientSessionHandler {
public:
  virtual ~ICSClientSessionHandler(){};

  virtual void OnEstablish(shared_ptr<CSClientSession> sess)                                                   = 0;
  virtual void OnTerminate(shared_ptr<CSClientSession> sess)                                                   = 0;
  virtual void OnHandlerMsg(shared_ptr<CSClientSession> sess, uint32_t msg_id, const char* data, uint32_t len) = 0;
  virtual void OnBeatHeartError(shared_ptr<CSClientSession> sess)                                              = 0;
};

class CSClientSession : public enable_shared_from_this<CSClientSession>, public CSession {
public:
  CSClientSession();
  virtual ~CSClientSession();

  void OnEstablish() override;
  void OnTerminate() override;
  void OnHandlerMsg(uint32_t msg_id, const char* attach, uint32_t attach_len, const char* msg, uint32_t msg_len);

  inline void SetHandler(shared_ptr<ICSClientSessionHandler> handler) {
    m_handler = handler;
  }

  inline void SendBytes(uint32_t msg_id, const char* msg, uint32_t len) {
    AsyncSendBytes(msg_id, msg, len, nullptr);
  }

  inline void SendProtoMsg(uint32_t msg_id, const google::protobuf::Message& msg) {
    AsyncSendProtoMsg(msg_id, msg, nullptr);
  }

private:
  shared_ptr<ICSClientSessionHandler> m_handler              = nullptr;
  shared_ptr<ITimerRegister>          m_timer_register       = nullptr;
  int64_t                             m_last_beat_heart_time = 0;

  enum eTimerID : uint32_t {
    BEAT_HEART_TIME_ID       = 1,
    SEND_BEAT_HEART_TIMER_ID = 2,
  };

  enum eTimerDelay : uint64_t {
    BEAT_HEART_TIME_DELAY       = 1000 * 10,
    SEND_BEAT_HEART_TIMER_DELAY = 1000 * 30,
  };

  const int64_t BEAT_HEART_MAX_TIME = 1000 * 60;
};

class CSClientSessionMgr : public enable_shared_from_this<CSClientSessionMgr>, public ISessionFactory {
public:
  shared_ptr<ISession> CreateSession() override;

  void AddSession(shared_ptr<ISession> session_ptr) override;

  shared_ptr<ISession> FindSessionBySessionID(uint64_t session_id);

  void RemoveSession(uint64_t session_id) override;

  bool SendProtoMsgBySessId(uint64_t session_id, uint32_t msg_id, const google::protobuf::Message& msg);

  void Connect(const string& host, uint32_t port, shared_ptr<ICSClientSessionHandler> handler, shared_ptr<ICoder> coder);

  void Listen(const string& ip, uint32_t port, shared_ptr<ICSClientSessionHandler> handler, shared_ptr<ICoder> coder, shared_ptr<IOContextPool> io_context_pool);

private:
  inline void setCoder(shared_ptr<ICoder> coder) {
    m_coder = coder;
  }

  inline void setHandler(shared_ptr<ICSClientSessionHandler> handler) {
    m_handler = handler;
  }

private:
  uint64_t                            m_session_id = 0;
  map<uint64_t, shared_ptr<ISession>> m_session_map;
  shared_ptr<ICSClientSessionHandler> m_handler = nullptr;
  shared_ptr<ICoder>                  m_coder   = nullptr;
  shared_ptr<Listener>                m_listen  = nullptr;
};

extern shared_ptr<CSClientSessionMgr> GCSClientSessionMgr;