/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-24 14:17:34
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-01-07 10:53:10
 */

#include "engine/inc/common/object_pool.h"
#include <map>
#include <mutex>

using namespace std;

namespace Framework {

class ObjectPoolMgr {
public:
  ObjectPoolMgr(bool safe_thread_flag) {
    m_safe_thread_flag = safe_thread_flag;
  }

public:
  template <typename T>
  ObjectPool<T>* GetObjectPool() {
    ObjectPool<T>* pool     = nullptr;
    size_t         pool_key = typeid(T).hash_code();
    ENTER_LOCK();
    auto iter = m_object_pool_map.find(pool_key);
    if (iter == m_object_pool_map.end()) {
      pool = new ObjectPool<T>(m_safe_thread_flag);
      m_object_pool_map.insert(make_pair(pool_key, pool));
      LogInfoA("[ObjectPool] Create {} ObjectPool", typeid(T).name());
    } else {
      pool = (ObjectPool<T>*)iter->second;
    }
    LEAVE_LOCK();

    return pool;
  }

  void Gc();

  void Monitor();

private:
  map<size_t, IObjectPool*> m_object_pool_map;
  mutex                     m_mutex;
  bool                      m_safe_thread_flag;
};

extern shared_ptr<ObjectPoolMgr> GObjectPoolMgr;

}  // namespace Framework