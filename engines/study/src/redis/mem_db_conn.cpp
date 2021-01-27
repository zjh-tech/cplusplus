/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-26 11:52:02
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-04-29 10:00:18
 */

#include "engine/inc/redis/mem_db_conn.h"
#include "engine/inc/log/logger.h"

using namespace Framework;

namespace Framework {
namespace MemDB {

  CMemDBConn::CMemDBConn(SMemDBConnInfo conn_info, asio::io_context& io_context)
    : m_conn_info(conn_info)
    , m_io_context(io_context) {
    m_errstr = new char[REDIS_REPLY_MAX_ERR_STR_LEN + 1];
  }

  CMemDBConn::~CMemDBConn() {
    if (m_errstr) {
      delete m_errstr;
      m_errstr = nullptr;
    }
  }

  bool CMemDBConn::connect() {
    struct timeval timeout = {1, 500000};  //1.5 seconds
    m_redis_context        = redisConnectWithTimeout(m_conn_info.host.c_str(), m_conn_info.port, timeout);

    if (m_redis_context == nullptr) {
      LogErrorA("[redis] connect redis_context nullptr");
      return false;
    }

    if (m_redis_context && m_redis_context->err) {
      LogErrorA("[redis] connect error: {} {}", m_redis_context->err, m_redis_context->errstr);
      FREE_REDIS_CONTEXT();
      return false;
    }

    LogInfoA("[redis] connect {} {} success ", m_conn_info.host, m_conn_info.port);

    if (!auth(m_conn_info.password)) {
      LogErrorA("[redis] connect {} {} auth error", m_conn_info.host, m_conn_info.port);
      return false;
    }

    LogInfoA("[redis] connect {} {} auth success", m_conn_info.host, m_conn_info.port);
    return true;
  }

  void CMemDBConn::disconnect() {
    FREE_REDIS_CONTEXT();
  }

  bool CMemDBConn::is_valid_handle() {
    return m_redis_context ? true : false;
  }

  bool CMemDBConn::auth(std::string& password) {
    if (password == "") {
      return false;
    }

    std::string auth_cmd = "AUTH  " + password;
    redisReply* reply    = (redisReply*)redisCommand(m_redis_context, auth_cmd.c_str());
    if (reply && reply->type == REDIS_REPLY_STATUS && Utils::StrCmp(reply->str, "OK") == 0) {
      FREE_REPLY(reply);
      return true;
    }

    FREE_REPLY(reply);
    return false;
  }

  void CMemDBConn::begin_transaction() {
    exec("MULTI");
  }

  void CMemDBConn::commit_transaction() {
    exec("EXEC");
  }

  void CMemDBConn::rollback_transaction() {
    exec("DISCARD");
  }

  void CMemDBConn::SetErrInfo(const char* info) {
    if (m_errstr) {
      memset(m_errstr, 0, REDIS_REPLY_MAX_ERR_STR_LEN);
      strncpy(m_errstr, info, REDIS_REPLY_MAX_ERR_STR_LEN);
    }
  }

}  // namespace MemDB
}  // namespace Framework