#include "engine/inc/tcp/net.h"

#include "engine/inc/log/env.h"
#include "engine/inc/tcp/connectionmgr.h"
#include "engine/inc/tcp/event.h"
#include "engine/inc/tcp/iconnection.h"
#include "engine/inc/tcp/ievent.h"
#include "engine/inc/tcp/isession.h"

namespace Framework
{
    namespace Tcp
    {
        Net::Net() : io_worker_(io_context_)
        {
        }

        void Net::PushEvent(shared_ptr<IEvent> event)
        {
            asio::post(io_context_, [event]() {
                if (event == nullptr)
                {
                    return;
                }

                shared_ptr<IConnection> conn_ptr = event->GetConnPtr();
                assert(conn_ptr);

                shared_ptr<ISession> session_ptr = event->GetSessPtr();
                assert(session_ptr);

                if (event->GetType() == eEventType::ConnEstablish)
                {
                    session_ptr->SetConnectionPtr(conn_ptr);
                    session_ptr->OnEstablish();
                }
                else if (event->GetType() == eEventType::RecvMsg)
                {
                    string& datas = event->GetDatas();
                    session_ptr->ProcessMsg(datas.data(), datas.size());
                }
                else if (event->GetType() == eEventType::ConnTerminate)
                {
                    ConnectionMgr::Instance()->Remove(conn_ptr->GetConnID());
                    session_ptr->OnTerminate();
                    session_ptr->SetConnectionPtr(nullptr);
                }
                else
                {
                    LogWarnA("[Net] PushEvent EventType={}", (uint32_t)event->GetType());
                }
            });
        }

        void Net::DoConnect(asio::io_context& io_context, const string& host, uint32_t port,
                            shared_ptr<ISession> session_ptr)
        {
            LogInfoA("[Net] DoConnect Host={}  Port={} ", host, port);

            assert(session_ptr);
            auto conn_ptr = ConnectionMgr::Instance()->Create(io_context, this, session_ptr);
            assert(conn_ptr);
            conn_ptr->SetSessionPtr(session_ptr);

            // host 可以是域名,也可以是IP
            tcp::resolver resolver(io_context);
            auto result = resolver.resolve(host, std::to_string(port));

            asio::async_connect(
                conn_ptr->GetSocket(), result,
                [this, conn_ptr, host, port, session_ptr](asio::error_code ec,
                                                          typename decltype(result)::endpoint_type endpoint) {
                    if (ec)
                    {
                        LogErrorA("[Net] DoConnect Host={} Port={} Error={} ", host, port, ec.message());
                        conn_ptr->Close(false);
                        return;
                    }

                    auto event_ptr = make_shared<Event>(eEventType::ConnEstablish, conn_ptr, session_ptr);
                    PushEvent(event_ptr);
                    LogInfoA("[Net] DoConnect Host={} Port={} Success", host, port);

                    conn_ptr->DoRead();
                });
        }

        bool Net::Run(uint32_t count)
        {
            if (io_context_.poll_one() == 0)
            {
                return false;
            }

            for (uint32_t i = 0; i < count; ++i)
            {
                if (io_context_.poll_one() == 0)
                {
                    break;
                }
            }

            return true;
        }

        void Net::Stop()
        {
            io_context_.stop();
            LogInfoA("[Net] Stop");
        }

    }  // namespace Tcp
}  // namespace Framework