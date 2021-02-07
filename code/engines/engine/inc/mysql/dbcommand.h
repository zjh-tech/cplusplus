/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-23 10:53:33
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-02-07 17:05:48
 */

#pragma once

#include "engine/inc/mysql/idbcommand.h"
#include <tuple>

namespace Framework {
namespace DB {

  class IDBRecordSet;

  class DBCommand : public IDBCommand {
  public:
    DBCommand(ExecSqlFunc execSqlFunc, ExecSqlRecordFunc execSqlRecordFunc) {
      execSqlFuncPtr       = execSqlFunc;
      execSqlRecordFuncPtr = execSqlRecordFunc;
    }
    virtual ~DBCommand(){};

    virtual void OnExecuteSql(shared_ptr<IDBConn> conn) {
      recordset = execSqlFuncPtr(conn);
    }

    virtual void OnExecuted() {
      execSqlRecordFuncPtr(get<0>(recordset), get<1>(recordset), get<2>(recordset));
    }

  private:
    ExecSqlFunc execSqlFuncPtr = nullptr;

    ExecSqlRecordFunc execSqlRecordFuncPtr = nullptr;

    tuple<int32_t, string, shared_ptr<IDBRecordSet>> recordset;
  };

}  // namespace DB
}  // namespace Framework