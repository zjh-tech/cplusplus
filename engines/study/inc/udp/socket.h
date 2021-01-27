/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-03-19 15:50:06
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-04-13 13:59:35
 */

#pragma once

#include "engine/inc/common/net/option.h"
#include "engine/inc/common/singleton.hpp"
#include <asio.hpp>
#include <memory>
#include <string>

using namespace asio::ip;
using asio::ip::udp;

namespace Framework {
namespace Udp {

  class CSession;

  class CSocket : public std::enable_shared_from_this<CSocket> {
  public:
    using SessionPtr = std::shared_ptr<CSession>;

    CSocket(asio::io_context& io_context, const char* local_ip, uint16_t local_port);
    virtual ~CSocket();

  public:
    void send_udp_msg(SessionPtr session, const char* msg, uint32_t len);

    void do_receive();

    static void print_qps(int64_t diff_time);

    asio::io_context& get_io_context();

  private:
    asio::io_context& m_io_context;
    udp::socket       m_socket;
    std::string       m_local_ip;
    uint16_t          m_local_port = 0;

    char          m_recv_buffer[MAX_PACKET_SIZE] = {0};
    udp::endpoint m_remote_endpoint;

    static std::atomic<int64_t> s_recv_qps_count;
  };

}  // namespace Udp
}  // namespace Framework
