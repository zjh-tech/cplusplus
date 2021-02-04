/*
 * @Author: zhengjinhong
 * @Date: 2019-11-11 10:44:56
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-02-04 11:39:41
 * @Description: file content
 */

#include "engine/inc/common/iocontextpool.h"
#include "engine/inc/log/env.h"
#include "engine/inc/mysql/db_field.h"
#include "engine/inc/mysql/env.h"
#include "engine/inc/mysql/sql_tool.h"
#include <tuple>

#define DB_LOOP_COUNT 100

using namespace std;

// CREATE TABLE `ACCOUNT` (
//     `accountid` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT '账号ID',
//     `username` varchar(128) NOT NULL DEFAULT '' COMMENT '账号',
//     `password` varchar(128) NOT NULL DEFAULT '' COMMENT '密码',
//     PRIMARY KEY (`accountid`),
//     KEY (`username`)
//     ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

int main(void) {
#if defined(__linux__)
  LOG_INIT("./log", "mysql_example", false, 1);
#else
  LOG_INIT("./log", "mysql_example", true, 1);
#endif

  DB_INIT()

  vector<DBConnSpec> conn_specs;
  DBConnSpec         spec;
  spec.DbName    = "test";
  spec.Host      = "127.0.0.1";
  spec.Port      = 3306;
  spec.User      = "root";
  spec.Password  = "123456";
  spec.Character = "utf8";
  conn_specs.push_back(spec);

  shared_ptr<IOContextPool> processor_pool = make_shared<IOContextPool>();
  processor_pool->Start(10, 100);

  if (GMysqlModule->Init(conn_specs, 1, 1, processor_pool) == false) {
    return -1;
  }

  GMysqlModule->AsyncExecCommand(
    1,
    [](shared_ptr<IMysqlConn> conn) -> tuple<int32_t, string, shared_ptr<IMysqlRecordSet>> {
      DBFieldPair fieldPair;
      string      username = "'zjh";
      string      password = "123456";
      fieldPair.AddField("username", username);
      fieldPair.AddField("password", password);

      SQLTool sqltool;
      string  insert_sql = sqltool.GetInsertSQL("ACCOUNT", &fieldPair);

      shared_ptr<IMysqlRecordSet> recordset = conn->ExecuteSql(insert_sql);
      return make_tuple(0, "", recordset);
    },
    [](int32_t errorcode, string errormsg, shared_ptr<IMysqlRecordSet> recordset) {
      if (errorcode != 0) {
        return;
      }

      LogInfoA("Insert Ok");
    });

  GMysqlModule->AsyncExecCommand(
    1,
    [](shared_ptr<IMysqlConn> conn) -> tuple<int32_t, string, shared_ptr<IMysqlRecordSet>> {
      string                      select_sql = "select accountid,password from ACCOUNT where username = 'zjh'";
      shared_ptr<IMysqlRecordSet> recordset  = conn->ExecuteSql(select_sql);
      return make_tuple(0, "", recordset);
    },
    [](int32_t errorcode, string errormsg, shared_ptr<IMysqlRecordSet> recordset) {
      if (errorcode != 0) {
        return;
      }

      LogInfoA("Select Ok");
    });

  bool busy      = false;
  auto db_module = GMysqlModule;
  while (true) {
    busy = false;

    if (db_module->Run(DB_LOOP_COUNT)) {
      busy = true;
    }

    if (!busy) {
      this_thread::sleep_for(chrono::milliseconds(1));
    }
  }

  DB_UNINIT()

  LOG_UNINIT();

  return 0;
}
