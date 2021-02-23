/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-26 11:42:16
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-02-07 16:05:58
 */

#pragma once
#include <string>

namespace Framework {
namespace MemDB {

  struct RedisConnSpec {
    std::string host     = "";
    uint16_t    port     = 0;
    std::string password = "";
    bool        flush    = true;
  };

#define MEMDB_ORDER_EXEC_ID 0

#define REDIS_REPLY_MAX_ERR_STR_LEN 128

}  // namespace MemDB
}  // namespace Framework
