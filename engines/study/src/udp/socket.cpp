/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-03-30 10:11:24
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-04-14 14:55:09
 */
#include "engine/inc/udp/socket.h"
#include "engine/inc/common/net/interface.h"
#include "engine/inc/common/net/main_module.h"
#include "engine/inc/common/net/net_stream.h"
#include "engine/inc/log/logger.h"
#include "engine/inc/udp/kcp_mgr.h"
#include "engine/inc/udp/session.h"
#include "engine/inc/udp/session_create_factory.h"
#include "engine/inc/udp/session_mgr.h"

using namespace Framework;

namespace Framework {
namespace Udp {

  std::atomic<int64_t> CSocket::s_recv_qps_count = 0;

  CSocket::CSocket(asio::io_context& io_context, const char* local_ip, uint16_t local_port)
    : m_io_context(io_context)
    , m_socket(m_io_context, udp::endpoint(asio::ip::address::from_string(local_ip), local_port))
    , m_local_ip(local_ip)
    , m_local_port(local_port) {
  }

  CSocket::~CSocket() {
  }

  void CSocket::do_receive() {
    auto self = this->shared_from_this();

    m_socket.async_receive_from(asio::buffer(m_recv_buffer, MAX_PACKET_SIZE), m_remote_endpoint, [this, self](std::error_code error, std::size_t bytes_transferred) {
      if (error) {
        LogErrorA("[Net]  async_receive_from error ");
        goto DO_RECV_FLAG;
      }

      if (bytes_transferred != 0) {
        //LogDebugA("[Net]  async_receive_from {} ", bytes_transferred);

        udp::endpoint remote_endpoint = m_remote_endpoint;
        asio::post(CMainModule::Instance()->get_io_context(), [this, self, remote_endpoint, recv_str = std::string(m_recv_buffer, bytes_transferred)]() {
          uint32_t conv = 0;
          if (ikcp_get_conv(recv_str.c_str(), recv_str.size(), &conv) != 0) {
            LogErrorA("[Net]  conv error ");
            return;
          }

          SessionPtr session = CKcpMgr::Instance()->GetSessionInPrepare(conv);
          if (!session) {
            session = CSessionMgr::Instance()->GetSessionPtr(conv);
          }

          assert(session);
          if (!session) {
            LogErrorA("[Net]  session not find");
            return;
          };

          session->kcp_input(recv_str.c_str(), recv_str.size());
          char one_package_buff[MAX_PACKET_SIZE] = {0};
          for (;;) {
            memset(one_package_buff, 0, MAX_PACKET_SIZE);
            int one_package_len = session->get_next_package(one_package_buff, MAX_PACKET_SIZE);
            if (one_package_len <= 0) {
              //LogDebugA("[Net]  sid = {} not package handler ", session->get_session_id());
              return;
            }

            //LogDebugA("[Net]  sid = {} handler msg {} ", session->get_session_id(), one_package_len);

            if (!session->get_verify_flag()) {
              // verify package
              auto verify_res_func = [session](uint32_t code) {
                NetStream verify_res;
                verify_res << code;
                session->send(verify_res.Data(), verify_res.Size());
              };

              NetStream stream(one_package_buff, one_package_len);
              uint64_t   server_type = 0;
              stream >> server_type;
              std::shared_ptr<ISessionVerify> session_verify = CSessionCreateFactory::Instance()->GetSessionVerify(server_type);
              assert(session_verify);
              NetStream verify_stream(one_package_buff, one_package_len);
              if (!session_verify->is_verify_req_ok(verify_stream)) {
                LogErrorA("[Net]  sid = {} verify error ", session->get_session_id());
                //verify_res_func(1);
                session->close();
                return;
              } else {
                //verify ok
                CSessionCreateFactory::Instance()->FillEmptySession(session, server_type);
                session->set_verify_flag_ok();
                session->set_remote_endpoint(remote_endpoint);
                session->set_socket(self);
                CKcpMgr::Instance()->DelPrepare(conv);
                CSessionMgr::Instance()->AddSession(session);
                LogInfoA("[Net]  sid = {} verify ok", session->get_session_id());
                //verify_res_func(0);
                continue;
              }
            }

            if (!session->get_verify_flag()) {
              LogErrorA("[Net]  sid = {} verify fail", session->get_session_id());
              session->close();
              return;
            }

            string                         data(one_package_buff, one_package_len);
            std::shared_ptr<ICodecHandler> codec       = session->get_codec_handler();
            std::shared_ptr<IMsgHandler>   msg_handler = session->get_msg_handler();

            //unzip
            if (!codec->unzip_body(data)) {
              LogErrorA("[Net] async_receive_from sid = {} unzip error ", session->get_session_id());
              continue;
            }

            //decode
            codec->decode_body(data);

            // if (codec->is_heart_beat_req_msg()) {
            //   LogDebugA("[Net] sid = {} Recv Ping", session->get_session_id());
            //   codec->SendHeartBeatResMsg(session);
            //   session->set_heartbeat_ok();
            //   continue;
            // }

            // if (codec->is_heart_beat_res_msg()) {
            //   LogDebugA("[Net] sid = {} Recv Pong", session->get_session_id());
            //   session->set_heartbeat_ok();
            //   continue;
            // }

            session->set_heartbeat_ok();
            ++s_recv_qps_count;
            msg_handler->handler_msg(session, data.c_str(), data.size());
          }
        });
      }

    DO_RECV_FLAG:
      do_receive();
    });
  }

  void CSocket::send_udp_msg(SessionPtr session, const char* msg, uint32_t len) {
    assert(session);
    auto        self = this->shared_from_this();
    std::string data(msg, len);
    asio::post(m_io_context, [this, self, session, send_str = std::move(data)]() {
      m_socket.async_send_to(asio::buffer(send_str.c_str(), send_str.size()), session->get_remote_endpoint(), [this, self](std::error_code error, std::size_t bytes_transferred) {
        if (error) {
          LogErrorA("[Net]  async_send_to error ");
          return;
        }

        //LogDebugA("[Net]  async_send_to {} ", bytes_transferred);
      });
    });
  }

  void CSocket::print_qps(int64_t diff_time) {
    auto recv_count = s_recv_qps_count.load();
    s_recv_qps_count.store(0);
    LogInfoA("QPS: recv {}, TotalCount: {} ,IntervalTime: {}", recv_count / diff_time, recv_count, diff_time);
  }

  asio::io_context& CSocket::get_io_context() {
    return m_io_context;
  }

}  // namespace Udp
}  // namespace Framework