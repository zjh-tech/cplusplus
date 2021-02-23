/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-26 11:47:50
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-02-19 14:15:21
 */

#pragma once

#include "engine/inc/redis/redisconn.h"

using namespace std;

namespace Framework {
namespace MemDB {

  class RedisModule {
  public:
  private:
    template <typename Cmd, typename... Args>
    RedisReplyUPtr command(RedisConn& conn, Cmd cmd, Args&&... args);
  };

  template <typename Cmd, typename... Args>
  RedisReplyUPtr RedisModule::command(RedisConn& conn, Cmd cmd, Args&&... args) {
    cmd(conn, forward<Args>(args)...);
    return conn.Recv();
  }
}  // namespace MemDB
}  // namespace Framework