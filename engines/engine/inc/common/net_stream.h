/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-03-20 20:30:13
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-01-22 11:35:44
 */

#pragma once

#include <cassert>
#include <cinttypes>
#include <cstring>
#include <string>

namespace Framework {

class NetStream {
public:
  NetStream();
  NetStream(const char* buff, size_t size);
  NetStream(const NetStream& r);
  virtual ~NetStream();

public:
  const char* Data();
  size_t      Size();

  void Reset();

  void EnsureCapacity(size_t len);

  static uint16_t n2hs(uint16_t value);
  static uint16_t h2ns(uint16_t value);
  static uint32_t h2nl(uint32_t val);
  static uint32_t n2hl(uint32_t val);
  static uint64_t h2nll(uint64_t val);
  static uint64_t n2hll(uint64_t val);

  template <typename T>
  T Read() {
    if constexpr (std::is_same_v<bool, T>) {
      //uint8_t bool same
      assert((m_rpos + sizeof(uint8_t)) <= m_wpos);
      uint8_t value = *(reinterpret_cast<uint8_t const*>(m_buff + m_rpos));
      m_rpos += sizeof(uint8_t);
      return (bool)value;
    } else if constexpr (std::is_same_v<uint8_t, T> || std::is_same_v<int8_t, T>) {
      assert((m_rpos + sizeof(T)) <= m_wpos);
      T value = *(reinterpret_cast<T const*>(m_buff + m_rpos));
      m_rpos += sizeof(T);
      return value;
    } else if constexpr (std::is_same_v<uint16_t, T> || std::is_same_v<int16_t, T>) {
      assert((m_rpos + sizeof(T)) <= m_wpos);
      T value = n2hs(*(reinterpret_cast<T const*>(m_buff + m_rpos)));
      m_rpos += sizeof(T);
      return value;
    } else if constexpr (std::is_same_v<uint32_t, T> || std::is_same_v<int32_t, T>) {
      assert((m_rpos + sizeof(T)) <= m_wpos);
      T value = n2hl(*(reinterpret_cast<T const*>(m_buff + m_rpos)));
      m_rpos += sizeof(T);
      return value;
    } else if constexpr (std::is_same_v<uint64_t, T> || std::is_same_v<int64_t, T>) {
      assert((m_rpos + sizeof(T)) <= m_wpos);
      T value = n2hll(*(reinterpret_cast<T const*>(m_buff + m_rpos)));
      m_rpos += sizeof(T);
      return value;
    } else if constexpr (std::is_same_v<std::string, T>) {
      assert((m_rpos + sizeof(uint16_t)) <= m_wpos);
      uint16_t len = n2hs(*(reinterpret_cast<uint16_t const*>(m_buff + m_rpos)));
      m_rpos += sizeof(uint16_t);
      assert((m_rpos + len) <= m_wpos);
      size_t pos = m_rpos;
      m_rpos += sizeof(len);
      return std::string(m_buff + pos, len);
    } else {
      assert(false);
      return T();
    }
  }

  template <typename T>
  NetStream* Write(T& value) {
    if constexpr (std::is_same_v<bool, T>) {
      //uint8_t bool same
      EnsureCapacity(sizeof(uint8_t));
      *(uint8_t*)(m_buff + m_wpos) = (uint8_t)value;
      m_wpos += sizeof(uint8_t);
    } else if constexpr (std::is_same_v<uint8_t, T> || std::is_same_v<int8_t, T>) {
      EnsureCapacity(sizeof(uint8_t));
      *(T*)(m_buff + m_wpos) = value;
      m_wpos += sizeof(uint8_t);
    } else if constexpr (std::is_same_v<uint16_t, T> || std::is_same_v<int16_t, T>) {
      EnsureCapacity(sizeof(uint16_t));
      *(T*)(m_buff + m_wpos) = h2ns(value);
      m_wpos += sizeof(uint16_t);
    } else if constexpr (std::is_same_v<uint32_t, T> || std::is_same_v<int32_t, T>) {
      EnsureCapacity(sizeof(uint32_t));
      *(T*)(m_buff + m_wpos) = h2nl(value);
      m_wpos += sizeof(uint32_t);
    } else if constexpr (std::is_same_v<uint64_t, T> || std::is_same_v<int64_t, T>) {
      EnsureCapacity(sizeof(uint64_t));
      *(T*)(m_buff + m_wpos) = h2nll(value);
      m_wpos += sizeof(uint64_t);
    } else if constexpr (std::is_same_v<std::string, T>) {
      size_t len = value.size();
      EnsureCapacity(sizeof(uint16_t) + len);
      *(uint16_t*)(m_buff + m_wpos) = h2ns(value);
      m_wpos += sizeof(uint16_t);
      memcpy(&m_buff[m_wpos], value.str(), len);
      m_wpos += len;
    } else {
      assert(false);
    }

    return this;
  }

  template <typename T>
  NetStream& operator>>(T& value) {
    value = Read<T>();
    return *this;
  }

  template <typename T>
  NetStream& operator<<(T& value) {
    return *(Write<T>(value));
  }

  void ReadBuff(char* pBuff, size_t len);

  void WriteBuff(const char* pBuff, size_t len);

  size_t GetRPos();

  size_t GetWPos();

private:
  char*  m_buff     = nullptr;
  size_t m_capacity = 0;
  size_t m_rpos     = 0;
  size_t m_wpos     = 0;

  static const size_t sDefaultSize = 64;
};

}  // namespace Framework