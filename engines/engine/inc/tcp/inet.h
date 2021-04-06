#pragma once
#include <asio.hpp>
#include <memory>

using namespace std;

namespace Framework
{
    namespace Tcp
    {
        class IEvent;
        class ISession;

        class INet
        {
        public:
            virtual ~INet(){};

            virtual void PushEvent(shared_ptr<IEvent> event) = 0;

            // host 可以是域名,也可以是IP
            virtual void DoConnect(asio::io_context& io_context, const string& host, uint32_t port, shared_ptr<ISession> session_ptr) = 0;

            virtual bool Run(uint32_t count) = 0;
        };
    }  // namespace Tcp
}  // namespace Framework