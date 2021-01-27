/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-03-31 15:37:39
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-04-13 20:24:24
 */

#include "engine/inc/udp/udp_server.h"
#include "engine/inc/common/net/processor_pool.h"
#include "engine/inc/udp/session_mgr.h"
#include "engine/inc/udp/socket.h"

namespace Framework {
namespace Udp {

  CUdpServer::CUdpServer(asio::io_context& io_context)
    : m_io_context(io_context) {
  }

  CUdpServer::~CUdpServer() {
  }

  void CUdpServer::Start(const char* ip, uint16_t port) {
    m_socket = std::make_shared<CSocket>(m_io_context, ip, port);
    assert(m_socket);
    m_socket->do_receive();
  }

  bool CUdpServer::Update() {
    //all kcp update
    CSessionMgr::Instance()->Update();
    return false;
  }

}  // namespace Udp
}  // namespace Framework