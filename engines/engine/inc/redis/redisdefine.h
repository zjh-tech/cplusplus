/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-26 11:42:16
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-02-18 16:26:29
 */

#pragma once
#include <string>

using namespace std;

namespace Framework {
namespace MemDB {

  struct RedisConnSpec {
    string   Host           = "";
    uint16_t Port           = 0;
    string   Password       = "";
    int64_t  ConnectTimeOut = 0;
    int64_t  CmdTimeOut     = 0;
  };

#define MEMDB_ORDER_EXEC_ID 0
#define REDIS_REPLY_MAX_ERR_STR_LEN 128

}  // namespace MemDB
}  // namespace Framework
