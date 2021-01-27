/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-03-31 20:08:38
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-04-13 20:23:20
 */

#include "engine/inc/udp/udp_client.h"
#include "engine/inc/common/net/processor_pool.h"
#include "engine/inc/udp/session.h"
#include "engine/inc/udp/session_create_factory.h"
#include "engine/inc/udp/session_mgr.h"
#include "engine/inc/udp/socket.h"

namespace Framework {
namespace Udp {

  CUdpClient::CUdpClient(asio::io_context& io_context)
    : m_io_context(io_context) {
  }

  CUdpClient::~CUdpClient() {
    if (m_session) {
      CSessionMgr::Instance()->DelSession(m_session->get_conv_id());
      m_session = nullptr;
    }
  }

  void CUdpClient::Start(const char* local_ip, uint16_t local_port, const char* remote_ip, uint16_t remote_port, uint64_t server_type, uint32_t conv) {
    m_conv = conv;

    m_socket = std::make_shared<CSocket>(m_io_context, local_ip, local_port);
    assert(m_socket);

    m_session = CSessionCreateFactory::Instance()->Create(server_type);
    assert(m_session);

    m_session->set_conv_id(m_conv);
    assert(m_session->start_kcp());
    udp::endpoint remote_endpoint(asio::ip::address::from_string(remote_ip), remote_port);
    m_session->set_remote_endpoint(remote_endpoint);
    m_session->set_socket(m_socket);
    m_session->set_verify_flag_ok();
    CSessionMgr::Instance()->AddSession(m_session);
    m_socket->do_receive();

    m_session->send_verify_req_msg();
  }

  SessionPtr CUdpClient::GetSessionPtr() {
    return m_session;
  }

  bool CUdpClient::Update() {
    m_session->kcp_update();

    return false;
  }

}  // namespace Udp
}  // namespace Framework