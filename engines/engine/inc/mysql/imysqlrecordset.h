/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-19 16:20:14
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-02-04 15:11:17
 */

#pragma once

#include <map>
#include <vector>

using namespace std;

namespace Framework {
namespace DB {
  class DBRecord;
  using DBRecordPtr = shared_ptr<DBRecord>;

  class IMysqlRecordSet {
  public:
    virtual ~IMysqlRecordSet(){};

    virtual const vector<DBRecordPtr>* GetDBRecordVec() = 0;
    virtual int64_t                    GetAffectRows()  = 0;
    virtual int64_t                    GetInsertId()    = 0;
  };
}  // namespace DB
}  // namespace Framework