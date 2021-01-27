/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-23 10:53:33
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-01-06 15:32:19
 */

#pragma once

#include "engine/inc/mysql/imysqlcommand.h"
#include <tuple>

namespace Framework {
namespace DB {

  class IMysqlRecordSet;

  class MysqlCommand : public IMysqlCommand {
  public:
    MysqlCommand(ExecSqlFunc execSqlFunc, ExecSqlRecordFunc execSqlRecordFunc) {
      execSqlFuncPtr       = execSqlFunc;
      execSqlRecordFuncPtr = execSqlRecordFunc;
    }
    virtual ~MysqlCommand(){};

    virtual void OnExecuteSql(shared_ptr<IMysqlConn> conn) {
      recordset = execSqlFuncPtr(conn);
    }

    virtual void OnExecuted() {
      execSqlRecordFuncPtr(get<0>(recordset), get<1>(recordset), get<2>(recordset));
    }

  private:
    ExecSqlFunc execSqlFuncPtr = nullptr;

    ExecSqlRecordFunc execSqlRecordFuncPtr = nullptr;

    tuple<int32_t, string, shared_ptr<IMysqlRecordSet>> recordset;
  };

}  // namespace DB
}  // namespace Framework