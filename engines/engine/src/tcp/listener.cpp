/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-12-15 11:08:03
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-12-24 15:14:40
 */

#include "engine/inc/tcp/listener.h"
#include "engine/inc/common/iocontextpool.h"
#include "engine/inc/log/env.h"
#include "engine/inc/tcp/connectionmgr.h"
#include "engine/inc/tcp/event.h"
#include "engine/inc/tcp/isession.h"
#include "engine/inc/tcp/net.h"

namespace Framework {
namespace Tcp {

  Listener::Listener(IOContextPoolPtr io_context_pool)
    : m_acceptor(Net::Instance()->GetIoContext()) {
    assert(io_context_pool);
    m_io_context_pool = io_context_pool;
  }

  bool Listener::start(const string& host, uint32_t port, shared_ptr<ISessionFactory> session_factory, ListenCbFunc fail_cb) {
    m_host            = host;
    m_port            = port;
    m_session_factory = session_factory;
    m_fail_cb_func    = fail_cb;
    LogInfoA("start listen {}  {}", m_host, m_port);

    asio::ip::tcp::endpoint endpoint(asio::ip::make_address(host), port);
    m_acceptor.open(endpoint.protocol());
    if (m_acceptor.is_open()) {
      m_acceptor.set_option(asio::socket_base::reuse_address(true));
      m_acceptor.non_blocking(true);
      //m_acceptor.set_option(asio::ip::tcp::no_delay(true));
      //asio::socket_base::send_buffer_size SNDBUF(2048);
      //asio::socket_base::receive_buffer_size RCVBUF(2048);
      //m_acceptor.set_option(SNDBUF);
      //m_acceptor.set_option(RCVBUF);
      this->m_acceptor.bind(endpoint);
      this->m_acceptor.listen();
      this->do_accept();
      return true;
    }

    return false;
  }

  void Listener::stop() {
    LogInfoA("stop listen {}  {}", m_host, m_port);
    m_acceptor.close();
  }

  void Listener::do_accept() {
    auto& iocontext       = m_io_context_pool->GetNextIoContext();
    auto  session_factory = m_session_factory.lock();
    assert(session_factory);
    if (session_factory == nullptr) {
      LogErrorA("[Net] AsyncAccept Host={}  Port={} SessionFactory Is Null", m_host, m_port);
      return;
    }
    auto session = session_factory->CreateSession();
    assert(session);
    shared_ptr<IConnection> conn = ConnectionMgr::Instance()->Create(iocontext, Net::Instance(), session);
    assert(conn);
    m_acceptor.async_accept(conn->GetSocket(), [this, conn, session](error_code ec) {
      if (ec) {
        LogErrorA("[Net] AsyncAccept Host={}  Port={} Error={}", m_host, m_port, ec.message());
        if (this->m_fail_cb_func) {
          this->m_fail_cb_func();
        }
        return;
      }

      auto eventPtr = make_shared<Event>(eEventType::ConnEstablish, conn, session);
      Net::Instance()->PushEvent(eventPtr);

      conn->DoRead();
      do_accept();
    });
  }

}  // namespace Tcp
}  // namespace Framework