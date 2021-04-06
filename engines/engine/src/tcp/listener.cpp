
#include "engine/inc/tcp/listener.h"

#include "engine/inc/common/iocontextpool.h"
#include "engine/inc/log/env.h"
#include "engine/inc/tcp/connectionmgr.h"
#include "engine/inc/tcp/event.h"
#include "engine/inc/tcp/isession.h"
#include "engine/inc/tcp/net.h"
#include "engine/inc/tcp/netdefine.h"

namespace Framework
{
    namespace Tcp
    {
        Listener::Listener(IOContextPoolPtr io_context_pool) : acceptor_(Net::Instance()->GetIoContext())
        {
            assert(io_context_pool);
            io_context_pool_ = io_context_pool;
        }

        bool Listener::Start(const string& host, uint32_t port, shared_ptr<ISessionFactory> session_factory, ListenCbFunc fail_cb_func)
        {
            host_            = host;
            port_            = port;
            session_factory_ = session_factory;
            fail_cb_func_    = fail_cb_func;
            LogInfoA("start listen {}  {}", host_, port_);

            asio::ip::tcp::endpoint endpoint(asio::ip::make_address(host), port);
            acceptor_.open(endpoint.protocol());
            if (acceptor_.is_open())
            {
                acceptor_.set_option(asio::socket_base::reuse_address(true));
                acceptor_.non_blocking(true);
                acceptor_.set_option(asio::ip::tcp::no_delay(true));
                asio::socket_base::send_buffer_size sendbuf(SEND_BUFF_SIZE);
                asio::socket_base::receive_buffer_size recvbuf(RECV_BUFF_SIZE);
                acceptor_.set_option(sendbuf);
                acceptor_.set_option(recvbuf);
                this->acceptor_.bind(endpoint);
                this->acceptor_.listen();
                this->do_accept();
                return true;
            }

            return false;
        }

        void Listener::Stop()
        {
            LogInfoA("stop listen {}  {}", host_, port_);
            acceptor_.close();
        }

        void Listener::do_accept()
        {
            auto& iocontext      = io_context_pool_->GetNextIoContext();
            auto session_factory = session_factory_.lock();
            assert(session_factory);
            if (session_factory == nullptr)
            {
                LogErrorA("[Net] AsyncAccept Host={}  Port={} SessionFactory Is Null", host_, port_);
                return;
            }
            auto session = session_factory->CreateSession();
            assert(session);
            shared_ptr<IConnection> conn = ConnectionMgr::Instance()->Create(iocontext, Net::Instance(), session);
            assert(conn);
            acceptor_.async_accept(conn->GetSocket(), [this, conn, session](error_code ec) {
                if (ec)
                {
                    LogErrorA("[Net] AsyncAccept Host={}  Port={} Error={}", host_, port_, ec.message());
                    if (this->fail_cb_func_)
                    {
                        this->fail_cb_func_();
                    }
                    return;
                }

                auto event_ptr = make_shared<Event>(eEventType::ConnEstablish, conn, session);
                Net::Instance()->PushEvent(event_ptr);

                conn->DoRead();
                do_accept();
            });
        }

    }  // namespace Tcp
}  // namespace Framework