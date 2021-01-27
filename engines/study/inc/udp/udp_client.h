/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-03-31 20:08:16
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-04-13 20:23:35
 */

#pragma once

#include "engine/inc/common/singleton.hpp"
#include <asio.hpp>
#include <thread>

using namespace std;
using namespace asio::ip;
using asio::ip::udp;

namespace Framework {
namespace Udp {

  class CSocket;
  class CSession;

  class CUdpClient : public Singleton<CUdpClient> {
  public:
    using SocketPtr  = std::shared_ptr<CSocket>;
    using SessionPtr = std::shared_ptr<CSession>;

    CUdpClient(asio::io_context& io_context);
    virtual ~CUdpClient();

  public:
    void Start(const char* local_ip, uint16_t local_port, const char* remote_ip, uint16_t remote_port, uint64_t server_type, uint32_t conv);

    bool Update();

    SessionPtr GetSessionPtr();

  private:
    bool run(uint32_t count);

  private:
    bool              m_is_running = false;
    asio::io_context& m_io_context;
    std::thread       m_thread;

    SocketPtr  m_socket  = nullptr;
    SessionPtr m_session = nullptr;
    uint32_t   m_conv    = 0;
  };

}  // namespace Udp
}  // namespace Framework
