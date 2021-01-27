/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-03-31 15:37:32
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-01-05 14:29:40
 */

#pragma once

#include "asio.hpp"
#include <thread>

namespace Framework {
namespace Udp {

  class CSocket;
  class CUdpServer {
  public:
    using SocketPtr = std::shared_ptr<CSocket>;

    CUdpServer(asio::io_context& io_context);
    virtual ~CUdpServer();

  public:
    void Start(const char* ip, uint16_t port);

    bool Update();

  private:
    bool              m_is_running = false;
    asio::io_context& m_io_context;
    std::thread       m_thread;

    SocketPtr m_socket = nullptr;
  };

}  // namespace Udp
}  // namespace Framework
