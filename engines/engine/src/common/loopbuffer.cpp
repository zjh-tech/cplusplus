/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-02-10 16:16:15
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-02-07 17:24:35
 */
#include "engine/inc/common/loopbuffer.h"

namespace Framework {

LoopBuffer::LoopBuffer(uint32_t capacity)
  : m_write_index(0)
  , m_read_index(0)
  , m_capacity(capacity) {
  m_data = static_cast<char*>(std::malloc(m_capacity));
}

LoopBuffer::~LoopBuffer() {
  free(m_data);
}

void LoopBuffer::Reset() {
  m_write_index = 0;
  m_read_index  = 0;
}

void LoopBuffer::Append(const void* data, uint32_t len) {
  uint32_t write_size = GetWriteLen();
  if (len > write_size) {
    uint32_t need_size = len - write_size;
    m_data             = static_cast<char*>(realloc(m_data, m_capacity + need_size));
    m_capacity += need_size;
  }

  std::memcpy(m_data + m_write_index, data, len);
  m_write_index += len;
}

void LoopBuffer::RefreshBuffer() {
  if (m_read_index == 0) {
    return;
  }

  uint32_t read_len = GetReadLen();
  if (read_len != 0) {
    memmove(m_data, GetReadAddr(), read_len);
  }
  m_read_index  = 0;
  m_write_index = read_len;
}

}  // namespace Framework