#pragma once
#include <assert.h>

#include <asio.hpp>
#include <memory>
#include <string>

using namespace std;
using asio::ip::tcp;
using namespace asio::ip;

namespace Framework
{
    namespace Tcp
    {
        class ISession;
        class INet;

        class IConnection
        {
        public:
            virtual ~IConnection(){};

            virtual uint64_t GetConnID()                                = 0;
            virtual shared_ptr<ISession> GetSessionPtr()                = 0;
            virtual void SetSessionPtr(shared_ptr<ISession> sessionPtr) = 0;
            virtual void AsyncSend(const char* msg, uint32_t len)       = 0;
            virtual void Terminate()                                    = 0;
            virtual void Close(bool autoFlag)                           = 0;
            virtual void DoRead()                                       = 0;
            virtual void DoWrite()                                      = 0;
            virtual tcp::socket& GetSocket()                            = 0;
        };

        class IConnectionMgr
        {
        public:
            virtual ~IConnectionMgr(){};

            virtual shared_ptr<IConnection> Create(asio::io_context& ioContext, INet* net,
                                                   shared_ptr<ISession> sess) = 0;
            virtual void Remove(uint64_t connID)                              = 0;
            virtual uint32_t GetConnCount()                                   = 0;
        };

    }  // namespace Tcp
}  // namespace Framework
