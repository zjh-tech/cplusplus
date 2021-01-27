/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-10-20 11:06:29
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-12-18 10:44:51
 */

#include "engine/inc/tcp/connectionmgr.h"
#include "engine/inc/log/env.h"
#include "engine/inc/tcp/connection.h"

namespace Framework {

namespace Tcp {

  shared_ptr<IConnection> ConnectionMgr::Create(asio::io_context& ioContext, INet* net, shared_ptr<ISession> sess) {
    ++m_connID;
    auto connID        = m_connID.load();
    auto connPtr       = make_shared<Connection>(ioContext, net, sess, connID);
    m_conn_map[connID] = connPtr;
    LogInfoA("[ConnectionMgr] Add ConnID={}", connID);
    return connPtr;
  }

  void ConnectionMgr::Remove(uint64_t connID) {
    auto iter = m_conn_map.find(connID);
    if (iter != m_conn_map.end()) {
      LogInfoA("[ConnectionMgr] Remove ConnID={}", connID);
      m_conn_map.erase(iter);
    }
  }

  uint32_t ConnectionMgr::GetConnCount() {
    return m_conn_map.size();
  }

}  // namespace Tcp
}  // namespace Framework