#include "engine/inc/tcp/connection.h"

#include "engine/inc/common/icoder.h"
#include "engine/inc/common/netstream.h"
#include "engine/inc/log/env.h"
#include "engine/inc/tcp/event.h"
#include "engine/inc/tcp/inet.h"
#include "engine/inc/tcp/isession.h"

using namespace Framework;

namespace Framework
{
    namespace Tcp
    {
        atomic<int64_t> Connection::s_send_qps_count = 0;
        atomic<int64_t> Connection::s_recv_qps_count = 0;

        Connection::Connection(asio::io_context& io_context, INet* net, shared_ptr<ISession> session_ptr,
                               uint64_t conn_id)
            : io_context_(io_context), socket_(io_context_)
        {
            session_ptr_ = session_ptr;
            net_ptr_     = net;
            conn_id_     = conn_id;
        }

        Connection::~Connection()
        {
        }

        void Connection::DoRead()
        {
            if (!socket_.is_open())
            {
                return;
            }

            auto self = this->shared_from_this();
            auto buf  = asio::buffer((char*)recv_buffer_ + recv_buffer_index_, MAX_MESSAGE_SIZE - recv_buffer_index_);
            socket_.async_read_some(buf, [this, self](error_code ec, size_t bytes_transferred) {
                recv_buffer_index_ += bytes_transferred;
                uint32_t read_index = 0;

                if (state_.load() != (int)ConnctionState::Establish)
                {
                    LogErrorA("[Net] ConnID={} AsyncReadSome Is Not ConnctionState::Establish ", conn_id_);
                    return;
                }

                if (socket_.is_open() == false)
                {
                    LogErrorA("[Net] ConnID={} AsyncReadSome Socket IsOpen Error", conn_id_);
                    return;
                }

                if (ec)
                {
                    LogErrorA("[Net] ConnID={} AsyncReadSome Socket IsOpen Error={}", conn_id_, ec.message());
                    Close(false);
                    return;
                }

                if (bytes_transferred == 0)
                {
                    LogWarnA("[Net] ConnID={} AsyncReadSome Transfered Bytes = 0", conn_id_);
                }

                auto sessionPtr = GetSessionPtr();
                if (sessionPtr == nullptr)
                {
                    LogErrorA("[Net] ConnID={} AsyncReadSome GetSessionPtr Error", conn_id_);
                    return;
                }

                auto coder = sessionPtr->GetCoder();

                if (coder == nullptr)
                {
                    LogErrorA("[Net] ConnID={} AsyncReadSome GetCoder Error", conn_id_);
                    return;
                }

                while (read_index < recv_buffer_index_)
                {
                    uint32_t header_len = coder->GetHeaderLen();
                    if (read_index + header_len > recv_buffer_index_)
                    {
                        LogDebugA("[Net] ConnID={} AsyncReadSome Not Enough Header", conn_id_);
                        goto DO_READ_FLAG;
                    }

                    uint32_t body_len              = coder->GetBodyLen((char*)recv_buffer_ + read_index);
                    uint32_t one_message_total_len = header_len + body_len;
                    if (one_message_total_len > MAX_MESSAGE_SIZE)
                    {
                        LogErrorA("[Net] ConnID={} Total Len={} Error", conn_id_, one_message_total_len);
                        Close(false);
                        return;
                    }

                    if (read_index + body_len > recv_buffer_index_)
                    {
                        LogDebugA("[Net] ConnID={} AsyncReadSome Not Enough Body", conn_id_);
                        goto DO_READ_FLAG;
                    }

                    read_index += header_len;

                    string temp_str(recv_buffer_ + read_index, body_len);
                    read_index += body_len;

                    // unzip
                    if (coder->UnzipBody(temp_str) == false)
                    {
                        LogErrorA("[Net] ConnID={} AsyncReadSome UnZip Error", conn_id_);
                        Close(false);
                        return;
                    }

                    // decode
                    coder->DecodeBody(temp_str);

                    auto evtPtr = make_shared<Event>(eEventType::RecvMsg, self, GetSessionPtr(), move(temp_str));
                    net_ptr_->PushEvent(evtPtr);

                    ++s_recv_qps_count;
                }

            DO_READ_FLAG:
                assert(read_index <= recv_buffer_index_);
                if (read_index != recv_buffer_index_)
                {
                    memmove(recv_buffer_, recv_buffer_ + read_index, recv_buffer_index_ - read_index);
                    recv_buffer_index_ -= read_index;
                }
                else
                {
                    recv_buffer_index_ = 0;
                }

                self->DoRead();
            });
        }

        void Connection::DoWrite()
        {
            if (is_send_finish())
            {
                return;
            }

            uint32_t two_send_vec_total_size = two_send_vec_ptr_->size();
            if (two_send_vec_total_size == two_send_vec_index_)
            {
                two_send_vec_ptr_   = send_vec_ptr_;
                two_send_vec_index_ = 0;
            }

            vector<asio::const_buffer> buffers;
            size_t total_size = 0;
            for (uint32_t i = two_send_vec_index_; i < two_send_vec_total_size; ++i)
            {
                auto& send_info = (*two_send_vec_ptr_)[i];
                total_size += send_info.size();
                if (total_size >= MAX_MESSAGE_SIZE)
                {
                    break;
                }

                ++two_send_vec_index_;
                buffers.emplace_back(send_info.data(), send_info.size());
            }
            auto self = this->shared_from_this();
            asio::async_write(socket_, buffers,
                              [this, self, total_size](asio::error_code ec, size_t bytes_transferred) {
                                  if (socket_.is_open() == false)
                                  {
                                      LogErrorA("[Net] ConnID={} AsyncWrite Socket IsOpen Error", conn_id_);
                                      return;
                                  }

                                  auto session = GetSessionPtr();
                                  if (session == nullptr)
                                  {
                                      LogErrorA("[Net] ConnID={} AsyncWrite GetSessionPtr Error", conn_id_);
                                      return;
                                  }

                                  if (ec)
                                  {
                                      LogErrorA("[Net] connID={} SessionID={} AsyncWrite  Error={} ", conn_id_,
                                                session->GetSessionID(), ec.message());
                                      Close(false);
                                      return;
                                  }

                                  if (bytes_transferred != total_size)
                                  {
                                      assert(false);
                                      LogErrorA("[Net] connID={} SessionID={} AsyncWrite  BytesTransfered Error ",
                                                conn_id_, session->GetSessionID());
                                      Close(false);
                                      return;
                                  }

                                  DoWrite();
                              });
        }

        void Connection::AsyncSend(const char* msg, uint32_t len)
        {
            auto self = this->shared_from_this();
            string str(msg, len);
            asio::post(io_context_, [data = move(str), this, self]() {
                if (!socket_.is_open())
                {
                    return;
                }

                if (state_.load() != (int)ConnctionState::Establish)
                {
                    return;
                }

                auto is_free_flag = send_vec_ptr_->empty() && two_send_vec_ptr_->empty();
                send_vec_ptr_->emplace_back(move(data));

                ++s_send_qps_count;
                if (is_free_flag)
                {
                    DoWrite();
                }
            });
        }

        void Connection::Terminate()
        {
            Close(true);
        }

        bool Connection::is_send_finish()
        {
            return (send_vec_ptr_->size() == 0 && two_send_vec_ptr_->size() == two_send_vec_index_) ? true : false;
        }

        void Connection::add_check_send_end_timer(int mill_sec, bool terminate)
        {
            check_send_end_timer_ = make_shared<asio::steady_timer>(socket_.get_executor());
            chrono::milliseconds time_long{mill_sec};
            check_send_end_timer_->expires_after(time_long);

            auto self = this->shared_from_this();
            check_send_end_timer_->async_wait([this, self, terminate](asio::error_code ec) {
                if (ec)
                {
                    return;
                }

                if (!terminate)
                {
                    close_socket();
                    return;
                }

                if (is_send_finish())
                {
                    close_socket();
                    LogInfoA("[Net] ConnID={} All Message Send Finish", conn_id_);
                    return;
                }

                add_check_send_end_timer(1000, terminate);
            });
        }

        void Connection::close_socket()
        {
            asio::error_code ignored_ec;
            socket_.shutdown(asio::ip::tcp::socket::shutdown_both, ignored_ec);
            socket_.close(ignored_ec);
        }

        void Connection::Close(bool autoFlag)
        {
            int old_state = (int)ConnctionState::Establish;
            if (state_.compare_exchange_strong(old_state, (int)ConnctionState::Closed) == false)
            {
                return;
            }

            if (autoFlag)
            {
                LogInfoA("[Net] ConnID={} Active Closed", conn_id_);
                //等待全部数据发送完成
            }
            else
            {
                LogInfoA("[Net] ConnID={} Passive Closed", conn_id_);
            }

            add_check_send_end_timer(0, autoFlag);

            auto self   = shared_from_this();
            auto evtPtr = make_shared<Event>(eEventType::ConnTerminate, self, GetSessionPtr());
            net_ptr_->PushEvent(evtPtr);
        }

        void Connection::PrintQps(int64_t diff_time)
        {
            auto send_count = s_send_qps_count.load();
            s_send_qps_count.store(0);

            auto recv_count = s_recv_qps_count.load();
            s_recv_qps_count.store(0);
            LogInfoA("QPS: recv {}, TotalCount: {} ,IntervalTime: {}", send_count / diff_time, send_count, diff_time);
            LogInfoA("QPS: send {}, TotalCount: {} ,IntervalTime: {}", recv_count / diff_time, recv_count, diff_time);
        }

    }  // namespace Tcp
}  // namespace Framework