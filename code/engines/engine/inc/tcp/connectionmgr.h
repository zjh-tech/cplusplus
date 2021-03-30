
#pragma once
#include <asio.hpp>
#include <map>
#include <memory>

#include "engine/inc/common/singleton.hpp"
#include "engine/inc/tcp/iconnection.h"
#include "engine/inc/tcp/inet.h"
#include "engine/inc/tcp/isession.h"

using namespace std;

namespace Framework
{
    namespace Tcp
    {
        class ConnectionMgr : public Singleton<ConnectionMgr>, public IConnectionMgr
        {
        public:
            shared_ptr<IConnection> Create(asio::io_context& io_context, INet* net, shared_ptr<ISession> sess) override;
            void Remove(uint64_t conn_id) override;
            uint32_t GetConnCount() override;

        private:
            atomic<uint64_t> conn_id_ = 0;
            map<uint64_t, shared_ptr<IConnection>> conn_map_;
        };

    }  // namespace Tcp
}  // namespace Framework