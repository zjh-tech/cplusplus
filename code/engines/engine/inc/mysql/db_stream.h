/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-14 14:35:01
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-01-28 18:44:02
 */

#pragma once

#include <cassert>
#include <cinttypes>
#include <cstring>
#include <memory>
#include <string>

using namespace std;

namespace Framework {
namespace DB {

  class IMysqlConn;

  class DBStream {
  public:
    DBStream();
    virtual ~DBStream();

  public:
    const char* Data();
    size_t      Size();

    void Reset();

    template <typename T>
    DBStream& operator<<(T& value) {
      ensure_capacity(sizeof(T));
      *(T*)(m_buff + m_wpos) = value;
      m_wpos += sizeof(T);
      return *this;
    }

    DBStream& operator<<(const char* value) {
      size_t value_len = strlen(value);
      ensure_capacity(value_len);
      memcpy(&m_buff[m_wpos], value, value_len);
      m_wpos += value_len;
      return *this;
    }

  private:
    void ensure_capacity(size_t len);

  private:
    char*  m_buff     = nullptr;
    size_t m_capacity = 0;
    size_t m_rpos     = 0;
    size_t m_wpos     = 0;

    static const size_t sDefaultSize = 64;
  };

  template <>
  inline DBStream& DBStream::operator<<(const string& value) {
    ensure_capacity(value.size());
    memcpy(&m_buff[m_wpos], value.c_str(), value.size());
    m_wpos += value.size();
    return *this;
  }

}  // namespace DB
}  // namespace Framework