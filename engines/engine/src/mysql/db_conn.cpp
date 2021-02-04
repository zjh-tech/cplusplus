

#include "engine/inc/mysql/db_conn.h"
#include "engine/inc/log/env.h"
#include "engine/inc/mysql/db_record_set.h"

namespace Framework {
namespace DB {

  MySQLConn::MySQLConn(DBConnSpec& info, asio::io_context& io_context)
    : m_db_conn_info(info)
    , m_io_context(io_context) {
    //mysql_thread_init();
  }

  MySQLConn::~MySQLConn() {
    release();
  }

  uint32_t MySQLConn::GetErrorCode() {
    return mysql_errno(m_pConnection);
  }

  std::string MySQLConn::GetErrorMsg() {
    return mysql_error(m_pConnection);
  }

  bool MySQLConn::Connect() {
    if (m_connected) {
      return true;
    }

    if (!m_pConnection) {
      m_pConnection = mysql_init(nullptr);
    }

    if (!m_pConnection) {
      return false;
    }

    if (!mysql_real_connect(m_pConnection, m_db_conn_info.Host.c_str(), m_db_conn_info.User.c_str(), m_db_conn_info.Password.c_str(), m_db_conn_info.DbName.c_str(), m_db_conn_info.Port, nullptr, 0)) {
      release();
      LogError("mysql_real_connect failed {} , {},{},{}", m_db_conn_info.Host.c_str(), m_db_conn_info.DbName.c_str(), GetErrorCode(), GetErrorMsg());
      return false;
    }

    m_connected = true;

    mysql_set_character_set(m_pConnection, m_db_conn_info.Character.c_str());

    char reconnect = 1;
    mysql_options(m_pConnection, MYSQL_OPT_RECONNECT, &reconnect);

    return true;
  }

  void MySQLConn::release() {
    if (m_pConnection) {
      mysql_close(m_pConnection);
      m_pConnection = nullptr;
    }

    m_connected = false;
    LogInfoA("[db] ip: {}  session {} close", m_db_conn_info.Host.c_str(), m_db_conn_info.DbName.c_str());
  }

  bool MySQLConn::isConnected() {
    return m_connected && (m_pConnection != nullptr);
  }

  string MySQLConn::EscapeString(const string& from) {
    string to;
    to.resize(from.size() * 2 + 1);
    size_t index = mysql_real_escape_string(m_pConnection, to.data(), from.c_str(), from.size());
    to.resize(index);
    return to;
  }

  shared_ptr<IMysqlRecordSet> MySQLConn::ExecuteSql(const string& sql) {
    shared_ptr<DBRecordSet> record_set_ptr = nullptr;
    if (!checkConnection()) {
      return record_set_ptr;
    }

    LogInfoA("[db] ExecuteSql {}", sql);

    //contain binary data must use mysql_real_query
    int code = mysql_real_query(m_pConnection, sql.c_str(), (unsigned long)sql.size());
    if (code != 0) {
      LogErrorA("[db] mysql_real_query {} error {} {} ", sql.c_str(), GetErrorCode(), GetErrorMsg());
      return record_set_ptr;
    }

    MYSQL_RES* mysql_res = mysql_store_result(m_pConnection);
    if (mysql_res == nullptr) {
      int64_t affect_rows = mysql_affected_rows(m_pConnection);
      int64_t insert_id   = mysql_insert_id(m_pConnection);
      record_set_ptr      = make_shared<DBRecordSet>();
      assert(record_set_ptr);

      record_set_ptr = make_shared<DBRecordSet>();
      assert(record_set_ptr);
      record_set_ptr->SetAffectRows(affect_rows);
      record_set_ptr->SetLastInsertId(insert_id);
    } else {
      //select sql
      record_set_ptr = make_shared<DBRecordSet>();
      assert(record_set_ptr);
      unsigned int field_count = mysql_num_fields(mysql_res);
      MYSQL_FIELD* fields      = mysql_fetch_fields(mysql_res);

      MYSQL_ROW mysql_row = nullptr;
      while ((mysql_row = mysql_fetch_row(mysql_res))) {
        unsigned long* lengths = mysql_fetch_lengths(mysql_res);

        auto record_ptr = make_shared<DBRecord>();
        for (unsigned int i = 0; i < field_count; ++i) {
          record_ptr->AddField(fields[i].name, mysql_row[i], lengths[i]);
        }
        record_set_ptr->AddDBRecord(record_ptr);
      }

      mysql_free_result(mysql_res);
      mysql_res = nullptr;
    }

    return record_set_ptr;
  }

  void MySQLConn::BeginTransact() {
    mysql_autocommit(m_pConnection, 0);
  }

  void MySQLConn::CommitTransact() {
    mysql_commit(m_pConnection);
    mysql_autocommit(m_pConnection, 1);
  }

  void MySQLConn::RollbackTransact() {
    mysql_rollback(m_pConnection);
    mysql_autocommit(m_pConnection, 1);
  }

  bool MySQLConn::checkConnection() {
    if (m_connected == false && m_pConnection != nullptr) {
      release();
    }

    if (m_pConnection == nullptr) {
      m_connected = Connect();
    }

    return m_connected;
  }

}  // namespace DB
}  // namespace Framework