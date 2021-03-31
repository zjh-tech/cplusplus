/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-26 11:48:56
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-02-24 17:53:12
 */

#include "engine/inc/redis/redismodule.h"
#include "engine/inc/log/env.h"

using namespace Framework;

namespace Framework {
namespace MemDB {

  RedisModule::RedisModule()
    : m_io_worker(m_io_context) {
  }

  bool RedisModule::Init(vector<RedisConnSpec>& conn_specs, uint32_t conn_max_count) {
    m_conn_specs     = conn_specs;
    m_conn_max_count = conn_max_count;

    if (m_conn_max_count == 0) {
      return false;
    }

    return true;
  }

  bool RedisModule::Run(uint32_t count) {
    return true;
  }

}  // namespace MemDB
}  // namespace Framework