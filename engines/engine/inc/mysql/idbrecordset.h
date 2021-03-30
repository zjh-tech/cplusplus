#pragma once

#include <map>
#include <vector>

using namespace std;

namespace Framework {
namespace DB {
  class DBRecord;
  using DBRecordPtr = shared_ptr<DBRecord>;

  class IDBRecordSet {
  public:
    virtual ~IDBRecordSet(){};

    virtual const vector<DBRecordPtr>* GetDBRecordVec() = 0;
    virtual int64_t                    GetAffectRows()  = 0;
    virtual int64_t                    GetInsertId()    = 0;
  };
}  // namespace DB
}  // namespace Framework