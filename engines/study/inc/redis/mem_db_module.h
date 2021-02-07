/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-26 11:47:50
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-04-30 15:51:58
 */

#pragma once

#include "engine/inc/common/net/main_module.h"
#include "engine/inc/common/net/processor_pool.h"
#include "engine/inc/log/logger.h"
#include "engine/inc/redis/mem_db_conn.h"
#include "engine/inc/redis/mem_db_define.h"
#include <string.h>

using namespace std;

namespace Framework {
class ProcessorPool;
}  // namespace Framework

namespace Framework {
namespace MemDB {

  class RedisConn;
  using RedisConnPtr = std::shared_ptr<RedisConn>;

  class RedisModule : public std::enable_shared_from_this<RedisModule> {
    using ProcessorPoolPtr = std::shared_ptr<ProcessorPool>;

  public:
    RedisModule(RedisConnSpec conn_info, uint32_t conn_count);
    virtual ~RedisModule();

  public:
    //sync method
    bool del_string(const char* key) {
      auto conn = choose_connection(MEMDB_ORDER_EXEC_ID);
      if (!conn) {
        LogErrorA("[redis] choose_connection {} error ", MEMDB_ORDER_EXEC_ID);
        return false;
      }
      return conn->command_bool("DEL %s", key);
    }

    template <typename T>
    bool get(const char* key, T& retval) {
      auto conn = choose_connection(MEMDB_ORDER_EXEC_ID);
      if (!conn) {
        LogErrorA("[redis] choose_connection {} error ", MEMDB_ORDER_EXEC_ID);
        return false;
      }
      string out_str;
      bool   bRet = conn->command_string(out_str, "GET %s", key);
      if (bRet) {
        retval = CConvert::str2t<T>(out_str.c_str());
      }

      return bRet;
    }

    template <typename T>
    bool set(const char* key, T value) {
      auto conn = choose_connection(MEMDB_ORDER_EXEC_ID);
      if (!conn) {
        LogErrorA("[redis] choose_connection {} error ", MEMDB_ORDER_EXEC_ID);
        return false;
      }

      string in_str = CConvert::str2t(value);
      return conn->command_status("SET %s %s", key, in_str.c_str());
    }

    bool get_bin(const char* key, string& retval) {
      auto conn = choose_connection(MEMDB_ORDER_EXEC_ID);
      if (!conn) {
        LogErrorA("[redis] choose_connection {} error ", MEMDB_ORDER_EXEC_ID);
        return false;
      }
      return conn->command_string(retval, "GET %s", key);
    }

    bool set_bin(const char* key, const char* buff, int len) {
      auto conn = choose_connection(MEMDB_ORDER_EXEC_ID);
      if (!conn) {
        LogErrorA("[redis] choose_connection {} error ", MEMDB_ORDER_EXEC_ID);
        return false;
      }
      return conn->command_status("SET  %s %b", key, buff, len);
    }

  public:
    //async method
    void del_string(int64_t uid, const char* key, std::function<void(bool)> cb) {
      auto conn = choose_connection(uid);
      if (!conn) {
        LogErrorA("[redis] choose_connection {} error ", uid);
        return;
      }

      asio::post(conn->get_io_context(), [this, conn, key, cb]() {
        bool bRet = conn->command_bool("DEL %s", key);
        asio::post(CMainModule::Instance()->get_io_context(), [this, bRet, cb]() {
          cb(bRet);
        });
      });
    }

    template <typename T>
    void get(int64_t uid, const char* key, std::function<void(bool, T)> cb) {
      auto conn = choose_connection(uid);
      if (!conn) {
        LogErrorA("[redis] choose_connection {} error ", uid);
        return;
      }

      asio::post(conn->get_io_context(), [this, conn, key, cb]() {
        string out_str;
        bool   bRet = conn->command_string(out_str, "GET %s", key);
        T      retval;
        if (bRet) {
          retval = CConvert::str2t<T>(out_str.c_str());
        }
        asio::post(CMainModule::Instance()->get_io_context(), [this, bRet, retval, cb]() {
          cb(bRet, retval);
        });
      });
    }

    template <typename T>
    void set(int64_t uid, const char* key, T value, std::function<void(bool)> cb) {
      auto conn = choose_connection(uid);
      if (!conn) {
        LogErrorA("[redis] choose_connection {} error ", uid);
        return;
      }

      string in_str = CConvert::str2t(value);
      asio::post(conn->get_io_context(), [this, conn, key, in_str, cb]() {
        bool bRet = conn->command_status("SET %s %s", key, in_str.c_str());
        asio::post(CMainModule::Instance()->get_io_context(), [this, bRet, cb]() {
          cb(bRet);
        });
      });
    }

    void get_bin(int64_t uid, const char* key, std::function<void(bool, const char* buff, size_t len)> cb) {
      auto conn = choose_connection(uid);
      if (!conn) {
        LogErrorA("[redis] choose_connection {} error ", uid);
        return;
      }

      asio::post(conn->get_io_context(), [this, conn, key, cb]() {
        string retval;
        bool   bRet = conn->command_string(retval, "GET %s", key);
        asio::post(CMainModule::Instance()->get_io_context(), [this, bRet, retval, cb]() {
          cb(bRet, retval.c_str(), retval.size());
        });
      });
    }

    void set_bin(int64_t uid, const char* key, const char* buff, int len, std::function<void(bool)> cb) {
      auto conn = choose_connection(uid);
      if (!conn) {
        LogErrorA("[redis] choose_connection {} error ", uid);
        return;
      }

      asio::post(conn->get_io_context(), [this, conn, key, buff, len, cb]() {
        bool bRet = conn->command_status("SET  %s %b", key, buff, len);
        asio::post(CMainModule::Instance()->get_io_context(), [this, bRet, cb]() {
          cb(bRet);
        });
      });
    }

  private:
    RedisConnPtr choose_connection(int64_t uid);

  private:
    RedisConnSpec             m_conn_info;
    uint32_t                  m_conn_count;
    std::vector<RedisConnPtr> m_conn_vec;
    ProcessorPoolPtr          m_processor_pool = nullptr;
  };  // namespace MemDB

}  // namespace MemDB
}  // namespace Framework