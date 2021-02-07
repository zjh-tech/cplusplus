/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-19 15:42:13
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-02-07 16:35:35
 */

#pragma once

#include "engine/inc/common/errorstring.h"
#include <asio.hpp>
#include <string>

using namespace std;
using namespace Framework;

namespace Framework {
namespace DB {

  class IDBRecordSet;

  class IDBConn {
  public:
    virtual ~IDBConn(){};

    virtual bool              Connect()      = 0;
    virtual asio::io_context& GetIoContext() = 0;

    virtual string                   EscapeString(const string& from) = 0;
    virtual shared_ptr<IDBRecordSet> ExecuteSql(const string& sql)    = 0;

    virtual uint32_t GetErrorCode() = 0;
    virtual string   GetErrorMsg()  = 0;

    virtual void BeginTransact()    = 0;
    virtual void CommitTransact()   = 0;
    virtual void RollbackTransact() = 0;
  };
}  // namespace DB
}  // namespace Framework