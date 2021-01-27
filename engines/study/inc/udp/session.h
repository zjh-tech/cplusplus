/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-03-18 20:43:58
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-04-13 16:47:18
 */
#pragma once

#include "engine/inc/common/net/interface.h"
#include "engine/inc/common/net/issession.h"
#include "engine/inc/common/net/option.h"
#include "engine/inc/common/singleton.hpp"
#include "engine/inc/udp/ikcp.h"
#include <asio.hpp>
#include <assert.h>
#include <memory>

using namespace std;
using namespace asio::ip;
using asio::ip::udp;

namespace Framework {
namespace Udp {

  class CSocket;

  class CSession : public ISession, public std::enable_shared_from_this<CSession> {
  public:
    friend class CSocket;
    friend class CUdpClient;
    using SocketPtr = std::shared_ptr<CSocket>;

    CSession(uint64_t server_type, uint64_t session_id);
    CSession(uint64_t session_id);  //empty session
    virtual ~CSession();

  public:
    //init
    void set_remote_endpoint(udp::endpoint remote_endpoint);
    void set_socket(SocketPtr socket);

    inline udp::endpoint& get_remote_endpoint() {
      return m_remote_endpoint;
    }

    uint32_t get_conv_id() const {
      return m_conv;
    }

    void set_conv_id(uint32_t conv);

  public:
    virtual bool send(const char* msg, uint32_t len);

  public:
    //kcp interface
    bool start_kcp();

    bool direct_send(const char* msg, uint32_t len);

    void kcp_input(const char* buff, uint32_t len);

    void kcp_update();

    int get_next_package(char* buff, uint32_t len);

    void send_udp_msg(const char* msg, uint32_t len);

    static int udp_output(const char* buf, int len, ikcpcb* kcp, void* user);

  private:
    //session verify
    bool get_verify_flag();
    void set_verify_flag_ok();
    void add_session_verify_timer();
    void send_verify_req_msg();

  private:
    void add_heartbeat_send_timer();
    void add_heartbeat_verify_timer();

    inline void set_heartbeat_ok() {
      m_heartbeat_flag = true;
    }

  private:
    void release();
    void close();

  private:
    uint32_t      m_conv  = 0;
    ikcpcb*       m_kcpcb = nullptr;
    udp::endpoint m_remote_endpoint;
    SocketPtr     m_socket = nullptr;

    bool                                m_verify_flag          = false;
    std::shared_ptr<asio::steady_timer> m_session_verify_timer = nullptr;

    bool m_heartbeat_flag = false;

    std::shared_ptr<asio::steady_timer> m_heartbeat_verify_timer = nullptr;
    std::shared_ptr<asio::steady_timer> m_heartbeat_send_timer   = nullptr;

    bool m_close_flag = false;
  };

  using SessionPtr     = std::shared_ptr<CSession>;
  using SessionWeakPtr = std::weak_ptr<CSession>;
}  // namespace Udp
}  // namespace Framework