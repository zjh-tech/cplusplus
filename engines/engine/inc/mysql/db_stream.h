/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-14 14:35:01
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-02-04 10:12:40
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
      string escape_string_str = escape_string(value);
      ensure_capacity(escape_string_str.size());
      memcpy(&m_buff[m_wpos], escape_string_str.c_str(), escape_string_str.size());
      m_wpos += escape_string_str.size();
      return *this;
    }

  private:
    void   ensure_capacity(size_t len);
    string escape_string(const string& from);

  private:
    char*  m_buff     = nullptr;
    size_t m_capacity = 0;
    size_t m_rpos     = 0;
    size_t m_wpos     = 0;

    static const size_t sDefaultSize = 64;
  };

  template <>
  inline DBStream& DBStream::operator<<(const string& value) {
    (*this) << value.c_str();
    return *this;
  }

  template <>
  inline DBStream& DBStream::operator<<(string& value) {
    (*this) << value.c_str();
    return *this;
  }

}  // namespace DB
}  // namespace Framework