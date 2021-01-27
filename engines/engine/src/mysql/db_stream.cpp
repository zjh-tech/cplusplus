/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-14 14:57:20
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-12-28 11:57:40
 */

#include "engine/inc/mysql/db_stream.h"
#include "engine/inc/mysql/imysqlconn.h"
#include <cstdlib>

namespace Framework {
namespace DB {

  DBStream::DBStream(shared_ptr<IMysqlConn> conn) {
    conn_ptr   = conn;
    m_buff     = static_cast<char*>(malloc(sDefaultSize));
    m_capacity = sDefaultSize;
  }

  DBStream::~DBStream() {
    if (conn_ptr) {
      conn_ptr = nullptr;
    }

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

  void DBStream::ensure_capacity(size_t len) {
    if (m_capacity < (m_wpos + len)) {
      m_buff     = static_cast<char*>(realloc(m_buff, m_capacity + len + sDefaultSize * 2));
      m_capacity = m_capacity + len + sDefaultSize;
    }
  }

  string DBStream::escape_string(const string& from) {
    return conn_ptr->EscapeString(from);
  }

}  // namespace DB
}  // namespace Framework