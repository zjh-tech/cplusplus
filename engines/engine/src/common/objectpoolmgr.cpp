#include "engine/inc/common/objectpoolmgr.h"

namespace Framework
{
    void ObjectPoolMgr::Gc()
    {
        LogInfoA("[ObjectPool] ObjectPool Start GC");

        ENTER_LOCK();
        for (auto iter = object_pool_map_.begin(); iter != object_pool_map_.end(); ++iter)
        {
            if (iter->second)
            {
                iter->second->Gc();
            }
        }
        LEAVE_LOCK();

        LogInfoA("[ObjectPool] ObjectPool End GC");
    }

    void ObjectPoolMgr::Monitor()
    {
        LogInfoA("[ObjectPool] ObjectPool Start Monitor Memory");

        ENTER_LOCK();
        for (auto iter = object_pool_map_.begin(); iter != object_pool_map_.end(); ++iter)
        {
            if (iter->second)
            {
                iter->second->Monitor();
            }
        }
        LEAVE_LOCK();

        LogInfoA("[ObjectPool] ObjectPool End Monitor Memory");
    }

    shared_ptr<ObjectPoolMgr> GObjectPoolMgr = nullptr;

}  // namespace Framework