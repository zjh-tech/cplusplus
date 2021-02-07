/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-26 11:48:56
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-04-28 19:22:36
 */

#include "engine/inc/redis/mem_db_module.h"
#include "engine/inc/common/convert.hpp"

using namespace Framework;

namespace Framework {
namespace MemDB {

  RedisModule::RedisModule(RedisConnSpec conn_info, uint32_t conn_count)
    : m_conn_info(conn_info)
    , m_conn_count(conn_count) {
    m_processor_pool = std::make_shared<ProcessorPool>();
    assert(m_processor_pool);
    if (conn_count == 0) {
      m_conn_count = std::thread::hardware_concurrency();
    }

    m_processor_pool->start(m_conn_count);

    for (uint32_t i = 0; i < conn_count; ++i) {
      RedisConnPtr conn_ptr = std::make_shared<RedisConn>(m_conn_info, m_processor_pool->get_processor()->get_io_context());
      assert(conn_ptr);
      if (!conn_ptr->connect()) {
        assert(false);
        return;
      }

      m_conn_vec.push_back(conn_ptr);
    }
  }

  RedisModule::~RedisModule() {
    m_conn_vec.clear();
    m_processor_pool = nullptr;
  }

  RedisConnPtr RedisModule::choose_connection(int64_t uid) {
    return m_conn_vec.size() == 0 ? nullptr : m_conn_vec[uid % m_conn_vec.size()];
  }

}  // namespace MemDB
}  // namespace Framework