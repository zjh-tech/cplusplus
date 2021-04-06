#include <map>
#include <mutex>

#include "engine/inc/common/objectpool.h"

using namespace std;

namespace Framework {
class ObjectPoolMgr {
public:
  ObjectPoolMgr(bool thread_safe_flag) {
    thread_safe_flag_ = thread_safe_flag;
  }

public:
  template <typename T>
  ObjectPool<T>* GetObjectPool() {
    ObjectPool<T>* pool     = nullptr;
    size_t         pool_key = typeid(T).hash_code();
    ENTER_LOCK();
    auto iter = object_pool_map_.find(pool_key);
    if (iter == object_pool_map_.end()) {
      pool = new ObjectPool<T>(thread_safe_flag_);
      object_pool_map_.insert(make_pair(pool_key, pool));
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
  map<size_t, IObjectPool*> object_pool_map_;
  mutex                     mutex_;
  bool                      thread_safe_flag_;
};

extern shared_ptr<ObjectPoolMgr> GObjectPoolMgr;

}  // namespace Framework