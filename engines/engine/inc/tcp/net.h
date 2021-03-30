#pragma once

#include <map>

#include "engine/inc/common/singleton.hpp"
#include "engine/inc/tcp/inet.h"

namespace Framework
{
    namespace Tcp
    {
        class Net : public Singleton<Net>, public INet
        {
        public:
            Net();

            asio::io_context& GetIoContext()
            {
                return io_context_;
            }

            void PushEvent(shared_ptr<IEvent> event) override;

            // host 可以是域名,也可以是IP
            void DoConnect(asio::io_context& io_context, const string& host, uint32_t port,
                           shared_ptr<ISession> session_ptr) override;

            bool Run(uint32_t count) override;

            void Stop();

        private:
            asio::io_context io_context_;
            asio::io_context::work io_worker_;
        };

    }  // namespace Tcp
}  // namespace Framework