/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-26 11:51:53
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-11-25 10:42:03
 */

#pragma once

#include "engine/inc/common/utils.hpp"
#include "engine/inc/common/var_type.hpp"
#include "engine/inc/redis/mem_db_define.h"
#include "hiredis.h"
#include <asio.hpp>
#include <map>
#include <memory>
#include <string.h>
#include <vector>

struct redisContext;
struct redisReply;

using namespace Framework;
using namespace std;

#define FREE_REDIS_CONTEXT()    \
  if (m_redis_context) {        \
    redisFree(m_redis_context); \
    m_redis_context = nullptr;  \
  }

#define FREE_REPLY(reply)   \
  if (reply) {              \
    freeReplyObject(reply); \
    reply = nullptr;        \
  }

namespace Framework {
namespace MemDB {

  class CMemDBConn : public std::enable_shared_from_this<CMemDBConn> {
  public:
    CMemDBConn(SMemDBConnInfo conn_info, asio::io_context& io_context);
    virtual ~CMemDBConn();

  public:
    bool connect();
    void disconnect();

    asio::io_context& get_io_context() {
      return m_io_context;
    }

  public:
    //redisvCommand
    bool exec(const char* cmd, ...) {
      if (!m_redis_context) {
        return false;
      }
      bool    bRet = false;
      va_list args;
      va_start(args, cmd);
      redisReply* reply = (redisReply*)(redisvCommand(m_redis_context, cmd, args));
      va_end(args);

      if (reply == nullptr || (reply && reply->type == REDIS_REPLY_ERROR)) {
        SetErrInfo(reply->str);
      } else {
        bRet = true;
      }

      FREE_REPLY(reply)
      return bRet;
    }

    //REDIS_REPLY_INTEGER
    bool command_bool(const char* cmd, ...) {
      if (!m_redis_context) {
        return false;
      }
      bool    bRet = false;
      va_list args;
      va_start(args, cmd);
      redisReply* reply = (redisReply*)(redisvCommand(m_redis_context, cmd, args));
      va_end(args);

      if (reply) {
        if (reply->type == REDIS_REPLY_INTEGER) {
          bRet = (reply->integer == 1) ? true : false;
        } else {
          SetErrInfo(reply->str);
        }
      }

      FREE_REPLY(reply)
      return bRet;
    }

    //REDIS_REPLY_INTEGER
    bool command_integer(int64_t& retval, const char* cmd, ...) {
      if (!m_redis_context) {
        return false;
      }
      bool    bRet = false;
      va_list args;
      va_start(args, cmd);
      redisReply* reply = (redisReply*)(redisvCommand(m_redis_context, cmd, args));
      va_end(args);

      if (reply) {
        if (reply->type == REDIS_REPLY_INTEGER) {
          retval = reply->integer;
          bRet   = true;
        } else {
          SetErrInfo(reply->str);
        }
      }

      FREE_REPLY(reply)
      return bRet;
    }

    //REDIS_REPLY_STATUS
    bool command_status(const char* cmd, ...) {
      if (!m_redis_context) {
        return false;
      }
      bool    bRet = false;
      va_list args;
      va_start(args, cmd);
      redisReply* reply = (redisReply*)(redisvCommand(m_redis_context, cmd, args));
      va_end(args);

      if (reply) {
        if (reply->type == REDIS_REPLY_STATUS && Utils::StrCmp(reply->str, "OK") == 0) {
          bRet = true;
        } else {
          SetErrInfo(reply->str);
        }
      }

      FREE_REPLY(reply)
      return bRet;
    }

    //REDIS_REPLY_STRING
    bool command_string(string& retval, const char* cmd, ...) {
      if (!m_redis_context) {
        return false;
      }
      bool bRet = false;

      va_list args;
      va_start(args, cmd);
      redisReply* reply = (redisReply*)(redisvCommand(m_redis_context, cmd, args));
      va_end(args);

      if (reply) {
        if (reply->type == REDIS_REPLY_STRING) {
          retval.append(reply->str, 0, reply->len);
          bRet = true;
        } else {
          SetErrInfo(reply->str);
        }
      }

      FREE_REPLY(reply)
      return bRet;
    }

    //REDIS_REPLY_ARRAY HKEYS,HVALS,SRANDMEMBER ...
    bool command_vector(vector<string>& retvec, const char* cmd, ...) {
      if (!m_redis_context) {
        return false;
      }
      bool bRet = false;

      va_list args;
      va_start(args, cmd);
      redisReply* reply = (redisReply*)(redisvCommand(m_redis_context, cmd, args));
      va_end(args);

      if (reply) {
        if (reply->type == REDIS_REPLY_ARRAY) {
          bRet = true;
          for (size_t i = 0; i < reply->elements; ++i) {
            retvec.emplace_back(string(reply->element[i]->str, reply->element[i]->len));
          }
        } else {
          SetErrInfo(reply->str);
        }
      }

      FREE_REPLY(reply)
      return bRet;
    }

    //REDIS_REPLY_ARRAY HGETALL...
    bool command_map(map<string, string>& retmap, const char* cmd, ...) {
      if (!m_redis_context) {
        return false;
      }
      bool bRet = false;

      va_list args;
      va_start(args, cmd);
      redisReply* reply = (redisReply*)(redisvCommand(m_redis_context, cmd, args));
      va_end(args);

      if (reply) {
        if (reply->type == REDIS_REPLY_ARRAY) {
          bRet = true;
          for (size_t i = 0; i < reply->elements && i + 1 < reply->elements; i += 2) {
            string key_str(reply->element[i]->str, reply->element[i]->len);
            string value_str(reply->element[i + 1]->str, reply->element[i + 1]->len);
            retmap[key_str] = value_str;
          }
        } else {
          SetErrInfo(reply->str);
        }
      }

      FREE_REPLY(reply)
      return bRet;
    }

  public:
    //redisCommandArgv

  private:
    void SetErrInfo(const char* info);

  public:
    void begin_transaction();
    void commit_transaction();
    void rollback_transaction();

  private:
    bool is_valid_handle();
    bool auth(std::string& password);

  private:
    SMemDBConnInfo    m_conn_info;
    asio::io_context& m_io_context;

    redisContext* m_redis_context = nullptr;
    char*         m_errstr        = nullptr;
  };

}  // namespace MemDB
}  // namespace Framework