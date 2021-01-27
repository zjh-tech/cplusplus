/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-22 16:40:54
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-01-07 10:51:09
 */

#pragma once

#include "engine/inc/common/list.h"
#include "engine/inc/log/env.h"
#include <assert.h>
#include <cinttypes>
#include <map>
#include <memory>
#include <mutex>
#include <typeinfo>

using namespace std;

namespace Framework {

class IObjectPool {
public:
  virtual ~IObjectPool(){};

public:
  virtual void Gc()      = 0;
  virtual void Monitor() = 0;
};

#define ENTER_LOCK()        \
  if (m_safe_thread_flag) { \
    m_mutex.lock();         \
  }

#define LEAVE_LOCK()        \
  if (m_safe_thread_flag) { \
    m_mutex.unlock();       \
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
  ObjectPool(bool safe_thread_flag) {
    m_safe_thread_flag = safe_thread_flag;
    INIT_LIST_HEAD(&m_block_list);
    INIT_LIST_HEAD(&m_free_list);
  }

  virtual ~ObjectPool() {
    ENTER_LOCK();
    INIT_LIST_HEAD(&m_free_list);

    list_head * pos, *n;
    TBlockInfo* block_info;
    list_for_each_safe(pos, n, &m_block_list) {
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
    //obj addr is valid
    bool        valid_addr_flag = false;
    list_head * pos, *n;
    TBlockInfo* block_info;
    list_for_each_safe(pos, n, &m_block_list) {
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
      list_add_tail(&t_info->list, &m_free_list);
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
    list_for_each_safe(block_pos, n, &m_block_list) {
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
        list_for_each(free_pos, &m_free_list) {
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

        if (m_grow_size > MIN_GROW_SIZE) {
          m_grow_size /= 2;
          LogInfoA("[ObjectPool]  {} ObjectPool Grow_Size = {}", typeid(T).name(), m_grow_size);
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
    list_for_each(block_pos, &m_block_list) {
      block_info = list_entry(block_pos, TBlockInfo, list);
      assert(block_info);
      if (block_info) {
        total_size += block_info->size;
      }
    }

    list_head* free_pos;
    TInfo*     t_info;
    list_for_each(free_pos, &m_free_list) {
      t_info = list_entry(free_pos, TInfo, list);
      assert(t_info);
      if (t_info) {
        ++free_size;
      }
    }

    LEAVE_LOCK();
    if (total_size != 0) {
      LogInfoA("[ObjectPool]  {} ObjectPool Monitor TotalSize ={},FreeSize = {},UsePrecent = {}", typeid(T).name(), total_size, free_size, (total_size - free_size) * 100 / total_size);
    }
  }

private:
  bool grow() {
    TInfo* array = (TInfo*)malloc(sizeof(TInfo) * m_grow_size);
    assert(array);
    if (!array) {
      return false;
    }

    TBlockInfo* block = (TBlockInfo*)malloc(sizeof(TBlockInfo));
    assert(block);
    if (!block) {
      return false;
    }

    for (int32_t i = 0; i < m_grow_size; ++i) {
      list_add_tail(&array[i].list, &m_free_list);
    }

    block->array = array;
    block->size  = m_grow_size;
    list_add_tail(&block->list, &m_block_list);

    m_grow_size *= 2;
    LogInfoA("[ObjectPool]  {} ObjectPool Grow_Size = {}", typeid(T).name(), m_grow_size);
    return true;
  }

  T* fetch() {
    ENTER_LOCK();
    if (list_empty(&m_free_list)) {
      if (!grow()) {
        LEAVE_LOCK();
        return nullptr;
      }
    }

    TInfo* t_info = list_first_entry(&m_free_list, TInfo, list);
    assert(t_info);
    list_del(&t_info->list);
    LEAVE_LOCK();

    t_info->magic_num = REFERENCE;
    return &t_info->t;
  }

private:
  struct list_head m_block_list;
  struct list_head m_free_list;

  int32_t m_grow_size = MIN_GROW_SIZE;

  mutex m_mutex;
  bool  m_safe_thread_flag;
};

}  // namespace Framework
