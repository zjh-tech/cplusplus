
#include "engine/inc/tcp/connectionmgr.h"

#include "engine/inc/log/env.h"
#include "engine/inc/tcp/connection.h"

namespace Framework
{
    namespace Tcp
    {
        shared_ptr<IConnection> ConnectionMgr::Create(asio::io_context& io_context, INet* net, shared_ptr<ISession> sess)
        {
            ++conn_id_;

            auto conn_id       = conn_id_.load();
            auto conn_ptr      = make_shared<Connection>(io_context, net, sess, conn_id);
            conn_map_[conn_id] = conn_ptr;
            LogInfoA("[ConnectionMgr] Add ConnID={}", conn_id);
            return conn_ptr;
        }

        void ConnectionMgr::Remove(uint64_t conn_id)
        {
            auto iter = conn_map_.find(conn_id);
            if (iter != conn_map_.end())
            {
                LogInfoA("[ConnectionMgr] Remove ConnID={}", conn_id);
                conn_map_.erase(iter);
            }
        }

        uint32_t ConnectionMgr::GetConnCount()
        {
            return conn_map_.size();
        }

    }  // namespace Tcp
}  // namespace Framework