/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-10-20 11:04:39
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-10-20 18:27:45
 */

#pragma once
#include "engine/inc/common/singleton.hpp"
#include "engine/inc/tcp/iconnection.h"
#include "engine/inc/tcp/inet.h"
#include "engine/inc/tcp/isession.h"
#include <asio.hpp>
#include <map>
#include <memory>

using namespace std;

namespace Framework {
namespace Tcp {

  class ConnectionMgr : public Singleton<ConnectionMgr>, public IConnectionMgr {
  public:
    shared_ptr<IConnection> Create(asio::io_context& ioContext, INet* net, shared_ptr<ISession> sess) override;
    void                    Remove(uint64_t connID) override;
    uint32_t                GetConnCount() override;

  private:
    atomic<uint64_t>                       m_connID = 0;
    map<uint64_t, shared_ptr<IConnection>> m_conn_map;
  };

}  // namespace Tcp
}  // namespace Framework