#pragma once

#include "engine/inc/common/singleton.hpp"
#include "engine/inc/tcp/env.h"

namespace Framework {
class ICoder;
}

namespace Framework {
namespace Tcp {
  class IConnection;
}
}  // namespace Framework

class CSession : public ISession {
public:
  virtual ~CSession() {}

  shared_ptr<IConnection> GetConnectionPtr() override {
    return m_conn_ptr.lock();
  }

  void SetConnectionPtr(shared_ptr<IConnection> connPtr) override {
    m_conn_ptr = connPtr;
  }

  uint64_t GetSessionID() override {
    return m_session_id;
  }

  void SetSessionID(uint64_t session_id) override {
    m_session_id = session_id;
  }

  shared_ptr<ICoder> GetCoder() override {
    return m_coder;
  }

  void SetCoder(shared_ptr<ICoder> coder) override {
    m_coder = coder;
  }

  shared_ptr<ISessionFactory> GetSessionFactory() override {
    return m_factory.lock();
  }

  void SetSessionFactory(shared_ptr<ISessionFactory> factotry) override {
    m_factory = factotry;
  }

  void Terminate() override;

  void SetListenSessionType() override {
    m_session_type = eSessionType::LISTEN;
  }

  bool IsListenSessionType() override {
    return (m_session_type == eSessionType::LISTEN) ? true : false;
  }

  void SetConnectSessionType() override {
    m_session_type = eSessionType::CONNECT;
  }

  bool IsConnectSessionType() override {
    return (m_session_type == eSessionType::CONNECT) ? true : false;
  }

  void SetAttachData(any attach) override {
    m_attach_data = attach;
  }

  any GetAttachData() override {
    return m_attach_data;
  }

  void ProcessMsg(const char* data, uint32_t len) override;

  virtual void OnHandlerMsg(uint32_t msg_id, const char* attach, uint32_t attach_len, const char* msg, uint32_t msg_len) = 0;

  void AsyncSendBytes(uint32_t msg_id, const char* msg, uint32_t len, IAttachParas* attach = nullptr) override;

  void AsyncSendProtoMsg(uint32_t msg_id, const google::protobuf::Message& msg, IAttachParas* attach = nullptr) override;

private:
  uint64_t     m_session_id   = 0;
  eSessionType m_session_type = eSessionType::LISTEN;

  weak_ptr<IConnection>     m_conn_ptr;
  shared_ptr<ICoder>        m_coder = nullptr;
  weak_ptr<ISessionFactory> m_factory;
  any                       m_attach_data;
};

#define PARSE_PROTO(oProto, arData, dwLen)           \
  if (!(arData))                                     \
    return false;                                    \
  if (!(oProto).ParseFromArray((arData), (dwLen))) { \
    return false;                                    \
  }
