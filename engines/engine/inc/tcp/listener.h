/*
 * @Author: zhengjinhong
 * @Date: 2019-11-12 13:56:34
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-12-24 14:59:35
 * @Description: file content
 */

#pragma once
#include <asio.hpp>
#include <memory>

namespace Framework {
class IOContextPool;
}  // namespace Framework

using namespace Framework;
using namespace std;

namespace Framework {
namespace Tcp {

  class ISessionFactory;

  typedef void (*ListenCbFunc)();

  class Listener {
    using IOContextPoolPtr = shared_ptr<IOContextPool>;

  public:
    Listener(IOContextPoolPtr io_context_pool);
    virtual ~Listener() {}

    bool start(const string& host, uint32_t port, shared_ptr<ISessionFactory> session_factory, ListenCbFunc fail_cb = nullptr);
    void stop();

  private:
    void do_accept();

    string                    m_host;
    int32_t                   m_port = 0;
    asio::ip::tcp::acceptor   m_acceptor;
    IOContextPoolPtr          m_io_context_pool = nullptr;
    weak_ptr<ISessionFactory> m_session_factory;
    ListenCbFunc              m_fail_cb_func = nullptr;
  };

}  // namespace Tcp
}  // namespace Framework
