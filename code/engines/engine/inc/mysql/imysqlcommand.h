/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-19 15:40:13
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-01-06 15:31:54
 */

#pragma once

#include <any>
#include <cinttypes>
#include <functional>
#include <memory>
#include <tuple>
#include <vector>

using namespace std;

namespace Framework {
namespace DB {

  class IMysqlConn;
  class IMysqlRecordSet;

  class IMysqlCommand {
  public:
    virtual ~IMysqlCommand(){};

    virtual void OnExecuteSql(shared_ptr<IMysqlConn> conn) = 0;
    virtual void OnExecuted()                              = 0;
  };

  using ExecSqlFunc       = function<tuple<int32_t, string, shared_ptr<IMysqlRecordSet>>(shared_ptr<IMysqlConn> conn)>;
  using ExecSqlRecordFunc = function<void(int32_t errorcode, string errormsg, shared_ptr<IMysqlRecordSet> recordset)>;

}  // namespace DB
}  // namespace Framework
