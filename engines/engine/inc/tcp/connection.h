/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-10-19 17:42:56
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-02-07 17:19:32
 */

#pragma once
#include "engine/inc/tcp/iconnection.h"
#include "engine/inc/tcp/netdefine.h"
#include <string>
#include <tuple>

using namespace std;

namespace Framework {
namespace Tcp {

  class ISession;

  class Connection : public enable_shared_from_this<Connection>, public IConnection {
  public:
    Connection(asio::io_context& ioContext, INet* net, shared_ptr<ISession> session_ptr, uint64_t conn_id);
    virtual ~Connection();

    static void PrintQps(int64_t diff_time);

  public:
    tuple<string, uint16_t> GetLocalEndPoint() {
      auto endpoint = m_socket.local_endpoint();
      return make_tuple(endpoint.address().to_string(), endpoint.port());
    }

    tuple<string, uint16_t> GetRemoteEndPoint() {
      auto endpoint = m_socket.remote_endpoint();
      return make_tuple(endpoint.address().to_string(), endpoint.port());
    }

    tcp::socket& GetSocket() override {
      return m_socket;
    }

    asio::io_context& GetIoContext() {
      return m_io_context;
    }

    shared_ptr<ISession> GetSessionPtr() override {
      return m_session_ptr.lock();
    }

    void SetSessionPtr(shared_ptr<ISession> session_ptr) {
      m_session_ptr = session_ptr;
    }

    uint64_t GetConnID() override {
      return m_conn_id;
    }

    void AsyncSend(const char* msg, uint32_t len) override;

    void Terminate() override;

    void DoRead() override;

    void DoWrite() override;

    void Close(bool autoFlag);

  private:
    void closeSocket();

    void addCheckSendEndTimer(int mill_sec, bool terminate);

  private:
    vector<string> m_send_vec;
    vector<string> m_mix_send_vec;

    char     m_recv_buffer[MAX_MESSAGE_SIZE] = {0};
    uint32_t m_recv_buffer_index             = 0;

    static atomic<int64_t> s_send_qps_count;
    static atomic<int64_t> s_recv_qps_count;

  private:
    asio::io_context&  m_io_context;
    tcp::socket        m_socket;
    weak_ptr<ISession> m_session_ptr;
    uint64_t           m_conn_id = 0;
    INet*              m_net_ptr = nullptr;

    enum class ConnctionState {
      Establish = 1,
      Closed    = 2,
    };
    atomic<int>                    m_state                = (int)ConnctionState::Establish;
    shared_ptr<asio::steady_timer> m_check_send_end_timer = nullptr;
  };

}  // namespace Tcp
}  // namespace Framework
