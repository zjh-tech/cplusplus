#pragma once
#include <asio.hpp>
#include <memory>

namespace Framework
{
    class IOContextPool;
}  // namespace Framework

using namespace Framework;
using namespace std;

namespace Framework
{
    namespace Tcp
    {
        class ISessionFactory;

        typedef void (*ListenCbFunc)();

        class Listener
        {
            using IOContextPoolPtr = shared_ptr<IOContextPool>;

        public:
            Listener(IOContextPoolPtr io_context_pool);
            virtual ~Listener()
            {
            }

            bool Start(const string& host, uint32_t port, shared_ptr<ISessionFactory> session_factory,
                       ListenCbFunc fail_cb_func = nullptr);
            void Stop();

        private:
            void do_accept();

            string host_;
            int32_t port_ = 0;
            asio::ip::tcp::acceptor acceptor_;
            IOContextPoolPtr io_context_pool_ = nullptr;
            weak_ptr<ISessionFactory> session_factory_;
            ListenCbFunc fail_cb_func_ = nullptr;
        };

    }  // namespace Tcp
}  // namespace Framework
