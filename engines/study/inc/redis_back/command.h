/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2021-02-19 10:48:34
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-02-19 12:07:49
 */

#pragma once

#include "engine/inc/redis/redisconn.h"

using namespace std;

namespace Framework {
namespace MemDB {

  namespace Cmd {
    inline void Get(RedisConn& conn, const string_view& key) {
      conn.Send("GET %b", key.data(), key.size());
    }
  }  // namespace Cmd

}  // namespace MemDB
}  // namespace Framework
