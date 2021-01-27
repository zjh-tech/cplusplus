/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-03-18 20:44:08
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-04-14 14:53:34
 */

#include "engine/inc/udp/session.h"
#include "engine/inc/common/net/interface.h"
#include "engine/inc/common/net/net_stream.h"
#include "engine/inc/common/utils.hpp"
#include "engine/inc/log/logger.h"
#include "engine/inc/udp/session_create_factory.h"
#include "engine/inc/udp/socket.h"
#include <sstream>

using namespace Framework;

namespace Framework {
namespace Udp {

  CSession::CSession(uint64_t server_type, uint64_t session_id) {
    m_server_type = server_type;
    m_session_id  = session_id;
  }

  CSession::CSession(uint64_t session_id) {
    m_session_id = session_id;
  }

  CSession::~CSession() {
    release();
  }

  bool CSession::start_kcp() {
    assert(m_conv);
    if (m_conv == 0) {
      return false;
    }

    m_kcpcb = ikcp_create(m_conv, (void*)this);
    assert(m_kcpcb);
    m_kcpcb->output = &CSession::udp_output;
    ikcp_nodelay(m_kcpcb, 1, 10, 2, 1);
    return true;
  }

  void CSession::set_conv_id(uint32_t conv) {
    m_conv = conv;
  }

  void CSession::set_remote_endpoint(udp::endpoint remote_endpoint) {
    m_remote_endpoint = remote_endpoint;
  }

  void CSession::set_socket(SocketPtr socket) {
    m_socket = socket;
  }

  bool CSession::send(const char* msg, uint32_t len) {
    if (!m_codec) {
      return false;
    }

    NetStream out_stream;
    m_codec->get_send_datas(msg, len, out_stream);
    if (ikcp_send(m_kcpcb, out_stream.Data(), out_stream.Size()) < 0) {
      LogErrorA("[Net] {} send_kcp_msg error", m_conv);
      return false;
    }
    //LogDebugA("[Net]  send_kcp_msg {} ", out_stream.Size());
    return true;
  }

  bool CSession::direct_send(const char* msg, uint32_t len) {
    if (ikcp_send(m_kcpcb, msg, len) < 0) {
      LogErrorA("[Net] {} send_kcp_msg error", m_conv);
      return false;
    }

    //LogDebugA("[Net]  send_kcp_msg {} ", len);
    return true;
  }

  int CSession::udp_output(const char* buff, int len, ikcpcb* kcp, void* user) {
    ((CSession*)user)->send_udp_msg(buff, len);
    return 0;
  }

  void CSession::send_udp_msg(const char* msg, uint32_t len) {
    auto self = this->shared_from_this();
    m_socket->send_udp_msg(self, msg, len);
  }

  void CSession::kcp_input(const char* buff, uint32_t len) {
    ikcp_input(m_kcpcb, buff, len);
  }

  void CSession::kcp_update() {
    uint32_t millisec = Utils::GetMillTime();
    ikcp_update(m_kcpcb, millisec);
  }

  int CSession::get_next_package(char* buff, uint32_t len) {
    return ikcp_recv(m_kcpcb, buff, len);
  }

  void CSession::send_verify_req_msg() {
    std::shared_ptr<ISessionVerify> session_verify = CSessionCreateFactory::Instance()->GetSessionVerify(m_server_type);
    assert(session_verify);
    NetStream out_stream;
    session_verify->fill_verify_req_info(m_server_type, out_stream);
    direct_send(out_stream.Data(), out_stream.Size());
  }

  bool CSession::get_verify_flag() {
    return m_verify_flag;
  }

  void CSession::set_verify_flag_ok() {
    m_verify_flag = true;
    if (m_session_verify_timer) {
      m_session_verify_timer->cancel();
    }
  }

  void CSession::add_session_verify_timer() {
    m_session_verify_timer = std::make_shared<asio::steady_timer>(m_socket->get_io_context());
    if (!m_session_verify_timer) {
      return;
    }

    std::chrono::milliseconds time_long{30000};  //30s
    m_session_verify_timer->expires_after(time_long);

    auto self = this->shared_from_this();
    m_session_verify_timer->async_wait([this, self](asio::error_code ec) {
      if (ec) {
        return;
      }

      if (!m_verify_flag) {
        LogInfoA("[Net] verify time out");
        //close();
      }
    });
  }

  void CSession::release() {
    m_codec       = nullptr;
    m_msg_handler = nullptr;
    m_socket      = nullptr;

    m_verify_flag = false;
    if (m_session_verify_timer) {
      m_session_verify_timer->cancel();
      m_session_verify_timer = nullptr;
    }

    if (m_heartbeat_send_timer) {
      m_heartbeat_send_timer->cancel();
      m_heartbeat_send_timer = nullptr;
    }

    if (m_heartbeat_verify_timer) {
      m_heartbeat_verify_timer->cancel();
      m_heartbeat_verify_timer = nullptr;
    }

    if (m_kcpcb) {
      ikcp_release(m_kcpcb);
      m_kcpcb = nullptr;
    }
  }

  void CSession::close() {
    if (m_close_flag) {
      return;
    }

    m_close_flag = true;
    release();
  }

  void CSession::add_heartbeat_send_timer() {
    if (m_heartbeat_send_timer == nullptr) {
      m_heartbeat_send_timer = std::make_shared<asio::steady_timer>(m_socket->get_io_context());
    }

    std::chrono::milliseconds time_long{20000};  //20s
    m_heartbeat_send_timer->expires_after(time_long);

    auto self = this->shared_from_this();
    m_heartbeat_send_timer->async_wait([this, self](asio::error_code ec) {
      if (ec) {
        return;
      }

      std::shared_ptr<ICodecHandler> codec = self->get_codec_handler();
      if (codec) {
        //codec->SendHeartBeatReqMsg(self);
      }

      add_heartbeat_send_timer();
    });
  }

  void CSession::add_heartbeat_verify_timer() {
    if (m_heartbeat_verify_timer == nullptr) {
      m_heartbeat_verify_timer = std::make_shared<asio::steady_timer>(m_socket->get_io_context());
    }

    std::chrono::milliseconds time_long{60000};  //60s
    m_heartbeat_verify_timer->expires_after(time_long);

    auto self = this->shared_from_this();
    m_heartbeat_verify_timer->async_wait([this, self](asio::error_code ec) {
      if (ec) {
        return;
      }

      if (m_heartbeat_flag) {
        LogInfoA("[Net] heartbeat success");
        m_heartbeat_flag = false;
        add_heartbeat_verify_timer();
      } else {
        LogInfoA("[Net] heartbeat fail");
        //close();
      }
    });
  }

}  // namespace Udp
}  // namespace Framework