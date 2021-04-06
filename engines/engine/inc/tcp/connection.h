#pragma once
#include <string>
#include <tuple>

#include "engine/inc/tcp/iconnection.h"
#include "engine/inc/tcp/netdefine.h"

using namespace std;

namespace Framework
{
    namespace Tcp
    {
        class ISession;

        class Connection : public enable_shared_from_this<Connection>, public IConnection
        {
        public:
            Connection(asio::io_context& io_context, INet* net, shared_ptr<ISession> session_ptr, uint64_t conn_id);
            virtual ~Connection();

            static void PrintQps(int64_t diff_time);

        public:
            tuple<string, uint16_t> GetLocalEndPoint()
            {
                auto endpoint = socket_.local_endpoint();
                return make_tuple(endpoint.address().to_string(), endpoint.port());
            }

            tuple<string, uint16_t> GetRemoteEndPoint()
            {
                auto endpoint = socket_.remote_endpoint();
                return make_tuple(endpoint.address().to_string(), endpoint.port());
            }

            tcp::socket& GetSocket() override
            {
                return socket_;
            }

            asio::io_context& GetIoContext()
            {
                return io_context_;
            }

            shared_ptr<ISession> GetSessionPtr() override
            {
                return session_ptr_.lock();
            }

            void SetSessionPtr(shared_ptr<ISession> session_ptr)
            {
                session_ptr_ = session_ptr;
            }

            uint64_t GetConnID() override
            {
                return conn_id_;
            }

            void AsyncSend(const char* msg, uint32_t len) override;

            void Terminate() override;

            void DoRead() override;

            void DoWrite() override;

            void Close(bool autoFlag);

        private:
            void close_socket();

            void add_check_send_end_timer(int mill_sec, bool terminate);

            bool is_send_finish();

        private:
            vector<string>* send_vec_ptr_;
            vector<string>* two_send_vec_ptr_;
            uint32_t two_send_vec_index_ = 0;

            char recv_buffer_[MAX_MESSAGE_SIZE] = {0};
            uint32_t recv_buffer_index_         = 0;

            static atomic<int64_t> s_send_qps_count;
            static atomic<int64_t> s_recv_qps_count;

        private:
            asio::io_context& io_context_;
            tcp::socket socket_;
            weak_ptr<ISession> session_ptr_;
            uint64_t conn_id_ = 0;
            INet* net_ptr_    = nullptr;

            enum class ConnctionState
            {
                Establish = 1,
                Closed    = 2,
            };
            atomic<int> state_                                   = (int)ConnctionState::Establish;
            shared_ptr<asio::steady_timer> check_send_end_timer_ = nullptr;
        };

    }  // namespace Tcp
}  // namespace Framework
