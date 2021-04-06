#pragma once

#include <assert.h>

#include <cinttypes>
#include <map>
#include <memory>
#include <mutex>
#include <typeinfo>

#include "engine/inc/common/list.h"
#include "engine/inc/log/env.h"

using namespace std;

namespace Framework {
class IObjectPool {
public:
  virtual ~IObjectPool(){};

public:
  virtual void Gc()      = 0;
  virtual void Monitor() = 0;
};

#define ENTER_LOCK()       \
  if (thread_safe_flag_) { \
    mutex_.lock();         \
  }

#define LEAVE_LOCK()       \
  if (thread_safe_flag_) { \
    mutex_.unlock();       \
  }

template <typename T>
class ObjectPool : public IObjectPool {
private:
#define REFERENCE 0xA110CAED
#define UN_REFERENCE 0xDEA110CA
#define MIN_GROW_SIZE 32

  struct TInfo {
    T                t;
    uint32_t         magic_num;
    struct list_head list;
  };

  struct TBlockInfo {
    TInfo*           array;
    int32_t          size;
    struct list_head list;
  };

public:
  ObjectPool(bool thread_safe_flag) {
    thread_safe_flag_ = thread_safe_flag;
    INIT_LIST_HEAD(&block_list_);
    INIT_LIST_HEAD(&free_list_);
  }

  virtual ~ObjectPool() {
    ENTER_LOCK();
    INIT_LIST_HEAD(&free_list_);

    list_head * pos, *n;
    TBlockInfo* block_info;
    list_for_each_safe(pos, n, &block_list_) {
      block_info = list_entry(pos, TBlockInfo, list);
      if (block_info) {
        free(block_info->array);
        free(block_info);
      }
    }
    LEAVE_LOCK();
  }

public:
  template <typename... Args>
  T* FetchObj(Args&&... args) {
    T* obj = fetch();
    if (!obj) {
      return nullptr;
    }

    memset(obj, 0, sizeof(T));
    new (obj) T(std::forward<Args>(args)...);
    return obj;
  }

  void ReleaseObj(T* obj) {
    if (!obj) {
      return;
    }

    TInfo* t_info = (TInfo*)obj;
    assert(t_info);
    if (!t_info) {
      return;
    }

    ENTER_LOCK();
    // obj addr is valid
    bool        valid_addr_flag = false;
    list_head * pos, *n;
    TBlockInfo* block_info;
    list_for_each_safe(pos, n, &block_list_) {
      block_info = list_entry(pos, TBlockInfo, list);
      assert(block_info);
      if (!block_info) {
        return;
      } else if (t_info >= block_info->array && t_info <= (block_info->array + block_info->size)) {
        valid_addr_flag = true;
        break;
      }
    }

    if (valid_addr_flag && t_info->magic_num == REFERENCE) {
      t_info->magic_num = UN_REFERENCE;
      t_info->t.~T();
      list_add_tail(&t_info->list, &free_list_);
    } else {
      if (!valid_addr_flag) {
        assert(valid_addr_flag);
        LogInfoA("[ObjectPool]  {} ObjectPool Valid Address", typeid(T).name());
      } else if (t_info->magic_num != REFERENCE) {
        assert(t_info->magic_num == REFERENCE);
        LogInfoA("[ObjectPool]  {} ObjectPool Many Times Release Object", typeid(T).name());
      }
    }
    LEAVE_LOCK();
  }

  void Gc() override {
    list_head * block_pos, *n;
    TBlockInfo* block_info;

    ENTER_LOCK();
    list_for_each_safe(block_pos, n, &block_list_) {
      block_info = list_entry(block_pos, TBlockInfo, list);
      assert(block_info);
      if (!block_info) {
        continue;
      }

      bool can_gc_flag = true;
      for (int32_t i = 0; i < block_info->size; ++i) {
        bool       is_in_free_list = false;
        list_head* free_pos;
        TInfo*     t_info;
        list_for_each(free_pos, &free_list_) {
          t_info = list_entry(free_pos, TInfo, list);
          if (block_info->array + i == t_info) {
            is_in_free_list = true;
            break;
          }
        }

        if (!is_in_free_list) {
          can_gc_flag = false;
          break;
        }
      }

      if (can_gc_flag) {
        for (int32_t i = 0; i < block_info->size; ++i) {
          TInfo* del_t_info = block_info->array + i;
          list_del_init(&del_t_info->list);
        }

        list_del_init(block_pos);
        free(block_info->array);
        free(block_info);

        if (grow_size_ > MIN_GROW_SIZE) {
          grow_size_ /= 2;
          LogInfoA("[ObjectPool]  {} ObjectPool Grow_Size = {}", typeid(T).name(), grow_size_);
        }
      }
    }
    LEAVE_LOCK();
  }

  void Monitor() override {
    int32_t     total_size = 0;
    int32_t     free_size  = 0;
    list_head*  block_pos;
    TBlockInfo* block_info;

    ENTER_LOCK();
    list_for_each(block_pos, &block_list_) {
      block_info = list_entry(block_pos, TBlockInfo, list);
      assert(block_info);
      if (block_info) {
        total_size += block_info->size;
      }
    }

    list_head* free_pos;
    TInfo*     t_info;
    list_for_each(free_pos, &free_list_) {
      t_info = list_entry(free_pos, TInfo, list);
      assert(t_info);
      if (t_info) {
        ++free_size;
      }
    }

    LEAVE_LOCK();
    if (total_size != 0) {
      LogInfoA("[ObjectPool]  {} ObjectPool Monitor TotalSize ={},FreeSize = {},UsePrecent = {}",
               typeid(T).name(),
               total_size,
               free_size,
               (total_size - free_size) * 100 / total_size);
    }
  }

private:
  bool grow() {
    TInfo* array = (TInfo*)malloc(sizeof(TInfo) * grow_size_);
    assert(array);
    if (!array) {
      return false;
    }

    TBlockInfo* block = (TBlockInfo*)malloc(sizeof(TBlockInfo));
    assert(block);
    if (!block) {
      return false;
    }

    for (int32_t i = 0; i < grow_size_; ++i) {
      list_add_tail(&array[i].list, &free_list_);
    }

    block->array = array;
    block->size  = grow_size_;
    list_add_tail(&block->list, &block_list_);

    grow_size_ *= 2;
    LogInfoA("[ObjectPool]  {} ObjectPool Grow_Size = {}", typeid(T).name(), grow_size_);
    return true;
  }

  T* fetch() {
    ENTER_LOCK();
    if (list_empty(&free_list_)) {
      if (!grow()) {
        LEAVE_LOCK();
        return nullptr;
      }
    }

    TInfo* t_info = list_first_entry(&free_list_, TInfo, list);
    assert(t_info);
    list_del(&t_info->list);
    LEAVE_LOCK();

    t_info->magic_num = REFERENCE;
    return &t_info->t;
  }

private:
  struct list_head block_list_;
  struct list_head free_list_;

  int32_t grow_size_ = MIN_GROW_SIZE;
  mutex   mutex_;
  bool    thread_safe_flag_;
};

}  // namespace Framework
