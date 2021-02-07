/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-10-19 17:52:46
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-02-07 17:30:19
 */

#include "engine/inc/tcp/connection.h"
#include "engine/inc/common/icoder.h"
#include "engine/inc/common/netstream.h"
#include "engine/inc/log/env.h"
#include "engine/inc/tcp/event.h"
#include "engine/inc/tcp/inet.h"
#include "engine/inc/tcp/isession.h"

using namespace Framework;

namespace Framework {
namespace Tcp {

  atomic<int64_t> Connection::s_send_qps_count = 0;
  atomic<int64_t> Connection::s_recv_qps_count = 0;

  Connection::Connection(asio::io_context& ioContext, INet* net, shared_ptr<ISession> session_ptr, uint64_t conn_id)
    : m_io_context(ioContext)
    , m_socket(m_io_context) {
    m_session_ptr = session_ptr;
    m_net_ptr     = net;
    m_conn_id     = conn_id;
  }

  Connection::~Connection() {
  }

  void Connection::DoRead() {
    if (!m_socket.is_open()) {
      return;
    }

    auto self = this->shared_from_this();
    auto buf  = asio::buffer((char*)m_recv_buffer + m_recv_buffer_index, MAX_MESSAGE_SIZE - m_recv_buffer_index);
    m_socket.async_read_some(buf, [this, self](error_code ec, size_t bytes_transferred) {
      m_recv_buffer_index += bytes_transferred;
      uint32_t read_index = 0;

      if (m_state.load() != (int)ConnctionState::Establish) {
        LogErrorA("[Net] ConnID={} AsyncReadSome Is Not ConnctionState::Establish ", m_conn_id);
        return;
      }

      if (m_socket.is_open() == false) {
        LogErrorA("[Net] ConnID={} AsyncReadSome Socket IsOpen Error", m_conn_id);
        return;
      }

      if (ec) {
        LogErrorA("[Net] ConnID={} AsyncReadSome Socket IsOpen Error={}", m_conn_id, ec.message());
        Close(false);
        return;
      }

      if (bytes_transferred == 0) {
        LogWarnA("[Net] ConnID={} AsyncReadSome Transfered Bytes = 0", m_conn_id);
      }

      auto sessionPtr = GetSessionPtr();
      if (sessionPtr == nullptr) {
        LogErrorA("[Net] ConnID={} AsyncReadSome GetSessionPtr Error", m_conn_id);
        return;
      }

      auto coder = sessionPtr->GetCoder();

      if (coder == nullptr) {
        LogErrorA("[Net] ConnID={} AsyncReadSome GetCoder Error", m_conn_id);
        return;
      }

      while (read_index < m_recv_buffer_index) {
        uint32_t header_len = coder->GetHeaderLen();
        if (read_index + header_len > m_recv_buffer_index) {
          LogDebugA("[Net] ConnID={} AsyncReadSome Not Enough Header", m_conn_id);
          goto DO_READ_FLAG;
        }

        uint32_t body_len = coder->GetBodyLen((char*)m_recv_buffer + read_index);
        if (read_index + body_len > m_recv_buffer_index) {
          LogDebugA("[Net] ConnID={} AsyncReadSome Not Enough Body", m_conn_id);
          goto DO_READ_FLAG;
        }

        read_index += header_len;

        string temp_str(m_recv_buffer + read_index, body_len);
        read_index += body_len;

        //unzip
        if (coder->UnzipBody(temp_str) == false) {
          LogErrorA("[Net] ConnID={} AsyncReadSome UnZip Error", m_conn_id);
          Close(false);
          return;
        }

        //decode
        coder->DecodeBody(temp_str);

        auto evtPtr = make_shared<Event>(eEventType::RecvMsg, self, GetSessionPtr(), move(temp_str));
        m_net_ptr->PushEvent(evtPtr);

        ++s_recv_qps_count;
      }

    DO_READ_FLAG:
      assert(read_index <= m_recv_buffer_index);
      if (read_index != m_recv_buffer_index) {
        memmove(m_recv_buffer, m_recv_buffer + read_index, m_recv_buffer_index - read_index);
        m_recv_buffer_index -= read_index;
      } else {
        m_recv_buffer_index = 0;
      }

      self->DoRead();
    });
  }

  void Connection::DoWrite() {
    if (m_send_vec.empty()) {
      return;
    }

    m_mix_send_vec.swap(m_send_vec);

    vector<asio::const_buffer> buffers;
    size_t                     total_size = 0;

    for (auto& data : m_mix_send_vec) {
      buffers.emplace_back(data.data(), data.size());
      total_size += data.size();

      if (total_size >= MAX_MESSAGE_SIZE) {
        break;
      }
    }

    auto self = this->shared_from_this();
    asio::async_write(m_socket, buffers, [this, self, total_size](asio::error_code ec, size_t bytes_transferred) {
      if (m_socket.is_open() == false) {
        LogErrorA("[Net] ConnID={} AsyncWrite Socket IsOpen Error", m_conn_id);
        return;
      }

      auto session = GetSessionPtr();
      if (session == nullptr) {
        LogErrorA("[Net] ConnID={} AsyncWrite GetSessionPtr Error", m_conn_id);
        return;
      }

      if (ec) {
        LogErrorA("[Net] connID={} SessionID={} AsyncWrite  Error={} ", m_conn_id, session->GetSessionID(), ec.message());
        Close(false);
        return;
      }

      if (bytes_transferred != total_size) {
        assert(false);
        LogErrorA("[Net] connID={} SessionID={} AsyncWrite  BytesTransfered Error ", m_conn_id, session->GetSessionID());
        Close(false);
        return;
      }

      m_mix_send_vec.clear();
      DoWrite();
    });
  }

  void Connection::AsyncSend(const char* msg, uint32_t len) {
    auto   self = this->shared_from_this();
    string str(msg, len);
    asio::post(m_io_context, [data = move(str), this, self]() {
      if (!m_socket.is_open()) {
        return;
      }

      if (m_state.load() != (int)ConnctionState::Establish) {
        return;
      }

      auto is_free_flag = m_send_vec.empty() && m_mix_send_vec.empty();
      m_send_vec.emplace_back(move(data));

      ++s_send_qps_count;
      if (is_free_flag) {
        DoWrite();
      }
    });
  }

  void Connection::Terminate() {
    Close(true);
  }

  void Connection::addCheckSendEndTimer(int mill_sec, bool terminate) {
    m_check_send_end_timer = make_shared<asio::steady_timer>(m_socket.get_executor());
    chrono::milliseconds time_long{mill_sec};
    m_check_send_end_timer->expires_after(time_long);

    auto self = this->shared_from_this();
    m_check_send_end_timer->async_wait([this, self, terminate](asio::error_code ec) {
      if (ec) {
        return;
      }

      if (!terminate) {
        closeSocket();
        return;
      }

      if (m_send_vec.size() == 0 && m_mix_send_vec.size()) {
        closeSocket();
        LogInfoA("[Net] ConnID={} All Message Send Finish", m_conn_id);
        return;
      }

      addCheckSendEndTimer(1000, terminate);
    });
  }

  void Connection::closeSocket() {
    asio::error_code ignored_ec;
    m_socket.shutdown(asio::ip::tcp::socket::shutdown_both, ignored_ec);
    m_socket.close(ignored_ec);
  }

  void Connection::Close(bool autoFlag) {
    int old_state = (int)ConnctionState::Establish;
    if (m_state.compare_exchange_strong(old_state, (int)ConnctionState::Closed) == false) {
      return;
    }

    if (autoFlag) {
      LogInfoA("[Net] ConnID={} Active Closed", m_conn_id);
      //等待全部数据发送完成
    } else {
      LogInfoA("[Net] ConnID={} Passive Closed", m_conn_id);
    }

    addCheckSendEndTimer(0, autoFlag);

    auto self   = shared_from_this();
    auto evtPtr = make_shared<Event>(eEventType::ConnTerminate, self, GetSessionPtr());
    m_net_ptr->PushEvent(evtPtr);
  }

  void Connection::PrintQps(int64_t diff_time) {
    auto send_count = s_send_qps_count.load();
    s_send_qps_count.store(0);

    auto recv_count = s_recv_qps_count.load();
    s_recv_qps_count.store(0);
    LogInfoA("QPS: recv {}, TotalCount: {} ,IntervalTime: {}", send_count / diff_time, send_count, diff_time);
    LogInfoA("QPS: send {}, TotalCount: {} ,IntervalTime: {}", recv_count / diff_time, recv_count, diff_time);
  }

}  // namespace Tcp
}  // namespace Framework