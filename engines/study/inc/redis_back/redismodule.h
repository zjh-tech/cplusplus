/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-26 11:47:50
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-02-25 11:42:19
 */

#pragma once

#include "asio.hpp"
#include "engine/inc/redis/redisconn.h"
#include <vector>

using namespace std;

namespace Framework {
namespace MemDB {

  class RedisModule {
  public:
    RedisModule();

    bool Init(vector<RedisConnSpec>& conn_specs, uint32_t conn_max_count);

    bool Run(uint32_t count);

  private:
    template <typename Cmd, typename... Args>
    RedisReplyUPtr command(RedisConn& conn, Cmd cmd, Args&&... args);

  private:
    asio::io_context       m_io_context;
    asio::io_context::work m_io_worker;

    vector<RedisConnSpec> m_conn_specs;
    uint32_t              m_conn_max_count = 0;
  };

  template <typename Cmd, typename... Args>
  RedisReplyUPtr RedisModule::command(RedisConn& conn, Cmd cmd, Args&&... args) {
    cmd(conn, forward<Args>(args)...);
    return conn.Recv();
  }

}  // namespace MemDB
}  // namespace Framework