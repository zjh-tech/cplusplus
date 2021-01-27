/*
 * @Author: zhengjinhong
 * @Date: 2019-11-11 13:55:27
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-11-25 11:53:40
 * @Description: file content
 */

#pragma once
#include <cinttypes>
#include <cstdlib>
#include <cstring>

using namespace std;

namespace Framework {

class LoopBuffer {
public:
  LoopBuffer(uint32_t capacity = 1024);
  ~LoopBuffer();

  void Reset();

  inline uint32_t GetCapacity() const {
    return m_capacity;
  }

  inline uint32_t GetReadLen() const {
    return m_write_index - m_read_index;
  }

  inline char* GetReadAddr() const {
    return m_data + m_read_index;
  }

  inline uint32_t GetWriteLen() const {
    return m_capacity - m_write_index;
  }

  inline char* GetWriteAddr() const {
    return m_data + m_write_index;
  }

  inline void AddReadSize(uint32_t read_size) {
    m_read_index += read_size;
  }

  void Append(const void* data, uint32_t len);

  void RefreshBuffer();

private:
  char*    m_data        = nullptr;
  uint32_t m_write_index = 0;
  uint32_t m_read_index  = 0;
  uint32_t m_capacity    = 0;
};

}  // namespace Framework
