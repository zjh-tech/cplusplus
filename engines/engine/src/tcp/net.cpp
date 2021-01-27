/*
 * @Author: zhengjinhong
 * @Date: 2019-11-12 16:33:34
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-01-07 11:57:59
 * @Description: file content
 */

#include "engine/inc/tcp/net.h"
#include "engine/inc/log/env.h"
#include "engine/inc/tcp/connectionmgr.h"
#include "engine/inc/tcp/event.h"
#include "engine/inc/tcp/iconnection.h"
#include "engine/inc/tcp/ievent.h"
#include "engine/inc/tcp/isession.h"

namespace Framework {
namespace Tcp {
  Net::Net()
    : m_io_worker(m_io_context) {
  }

  void Net::PushEvent(shared_ptr<IEvent> event) {
    asio::post(m_io_context, [event]() {
      if (event == nullptr) {
        return;
      }

      shared_ptr<IConnection> connPtr = event->GetConnPtr();
      assert(connPtr);

      shared_ptr<ISession> sessionPtr = event->GetSessPtr();
      assert(sessionPtr);

      if (event->GetType() == eEventType::ConnEstablish) {
        sessionPtr->SetConnectionPtr(connPtr);
        sessionPtr->OnEstablish();
      } else if (event->GetType() == eEventType::RecvMsg) {
        string& datas = event->GetDatas();
        sessionPtr->ProcessMsg(datas.data(), datas.size());
      } else if (event->GetType() == eEventType::ConnTerminate) {
        ConnectionMgr::Instance()->Remove(connPtr->GetConnID());
        sessionPtr->OnTerminate();
        sessionPtr->SetConnectionPtr(nullptr);
      } else {
        LogWarnA("[Net] PushEvent EventType={}", (uint32_t)event->GetType());
      }
    });
  }

  void Net::DoConnect(asio::io_context& io_context, const string& host, uint32_t port, shared_ptr<ISession> session_ptr) {
    LogInfoA("[Net] DoConnect Host={}  Port={} ", host, port);

    assert(session_ptr);
    auto connPtr = ConnectionMgr::Instance()->Create(io_context, this, session_ptr);
    assert(connPtr);
    connPtr->SetSessionPtr(session_ptr);

    //host 可以是域名,也可以是IP
    tcp::resolver resolver(io_context);
    auto          result = resolver.resolve(host, std::to_string(port));

    asio::async_connect(connPtr->GetSocket(), result, [this, connPtr, host, port, session_ptr](asio::error_code ec, typename decltype(result)::endpoint_type endpoint) {
      if (ec) {
        LogErrorA("[Net] DoConnect Host={} Port={} Error={} ", host, port, ec.message());
        connPtr->Close(false);
        return;
      }

      auto eventPtr = make_shared<Event>(eEventType::ConnEstablish, connPtr, session_ptr);
      PushEvent(eventPtr);
      LogInfoA("[Net] DoConnect Host={} Port={} Success", host, port);

      connPtr->DoRead();
    });
  }

  bool Net::Run(uint32_t count) {
    if (m_io_context.poll_one() == 0) {
      return false;
    }

    for (uint32_t i = 0; i < count; ++i) {
      if (m_io_context.poll_one() == 0) {
        break;
      }
    }

    return true;
  }

  void Net::Stop() {
    m_io_context.stop();
    LogInfoA("[Net] Stop");
  }

}  // namespace Tcp
}  // namespace Framework