/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-13 18:53:43
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-02-07 16:48:16
 */

#pragma once

#include "engine/inc/mysql/dbdefine.h"
#include "engine/inc/mysql/idbconn.h"
#include <list>
#include <mysql.h>

using namespace std;

namespace Framework {
namespace DB {

  class IDBRecordSet;

  class MySQLConn : public enable_shared_from_this<MySQLConn>, public IDBConn {
  public:
    MySQLConn(DBConnSpec& info, asio::io_context& context);
    virtual ~MySQLConn();

  public:
    bool              Connect() override;
    asio::io_context& GetIoContext() override {
      return m_io_context;
    }

    string                   EscapeString(const string& from) override;
    shared_ptr<IDBRecordSet> ExecuteSql(const string& sql) override;
    uint32_t                 GetErrorCode() override;
    string                   GetErrorMsg() override;

    void BeginTransact() override;
    void CommitTransact() override;
    void RollbackTransact() override;

  private:
    bool isConnected();
    bool checkConnection();
    void release();

  private:
    DBConnSpec        m_db_conn_info;
    asio::io_context& m_io_context;

    volatile bool m_connected = false;

    MYSQL* m_pConnection = nullptr;
  };

}  // namespace DB
}  // namespace Framework