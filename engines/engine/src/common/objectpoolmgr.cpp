/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-24 14:18:45
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-02-07 17:24:42
 */

#include "engine/inc/common/objectpoolmgr.h"

namespace Framework {

void ObjectPoolMgr::Gc() {
  LogInfoA("[ObjectPool] ObjectPool Start GC");

  ENTER_LOCK();
  for (auto iter = m_object_pool_map.begin(); iter != m_object_pool_map.end(); ++iter) {
    if (iter->second) {
      iter->second->Gc();
    }
  }
  LEAVE_LOCK();

  LogInfoA("[ObjectPool] ObjectPool End GC");
}

void ObjectPoolMgr::Monitor() {
  LogInfoA("[ObjectPool] ObjectPool Start Monitor Memory");

  ENTER_LOCK();
  for (auto iter = m_object_pool_map.begin(); iter != m_object_pool_map.end(); ++iter) {
    if (iter->second) {
      iter->second->Monitor();
    }
  }
  LEAVE_LOCK();

  LogInfoA("[ObjectPool] ObjectPool End Monitor Memory");
}

shared_ptr<ObjectPoolMgr> GObjectPoolMgr = nullptr;

}  // namespace Framework