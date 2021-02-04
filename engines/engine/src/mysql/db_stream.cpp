/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-14 14:57:20
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-02-03 18:23:57
 */

#include "engine/inc/mysql/db_stream.h"
#include "engine/inc/mysql/imysqlconn.h"
#include <cstdlib>

namespace Framework {
namespace DB {

  DBStream::DBStream() {
    m_buff     = static_cast<char*>(malloc(sDefaultSize));
    m_capacity = sDefaultSize;
  }

  DBStream::~DBStream() {
    if (m_buff) {
      free(m_buff);
      m_buff = nullptr;
    }
  }

  const char* DBStream::Data() {
    return m_buff;
  }

  size_t DBStream::Size() {
    return m_wpos;
  }

  void DBStream::Reset() {
    m_rpos = 0;
    m_wpos = 0;
  }

  string DBStream::escape_string(const string& from) {
    string to;
    to.resize(from.size() * 2 + 1);
    size_t index = 0;
    //method 1 :
    //index = mysql_real_escape_string(m_pConnection, to.data(), from.c_str(), from.size());
    //method 2 :
    size_t      from_len  = from.size();
    const char* from_data = from.c_str();
    for (size_t i = 0; i < from_len; ++i) {
      char c = from_data[i];
      switch (c) {
        case '\0':
          to[index++] = '\\';
          to[index++] = '0';
          break;
        case '\n':
          to[index++] = '\\';
          to[index++] = 'n';
          break;
        case '\r':
          to[index++] = '\\';
          to[index++] = 'r';
          break;
        case 0x1a:
          to[index++] = '\\';
          to[index++] = 'Z';
          break;
        case '\'':
          to[index++] = '\\';
          to[index++] = '\'';
          break;
        case '"':
          to[index++] = '\\';
          to[index++] = '\"';
          break;
        case '\\':
          to[index++] = '\\';
          to[index++] = '\\';
          break;
        default:
          to[index++] = c;
      }
    }
    to.resize(index);
    return to;
  }

  void DBStream::ensure_capacity(size_t len) {
    if (m_capacity < (m_wpos + len)) {
      m_buff     = static_cast<char*>(realloc(m_buff, m_capacity + len + sDefaultSize * 2));
      m_capacity = m_capacity + len + sDefaultSize;
    }
  }

}  // namespace DB
}  // namespace Framework