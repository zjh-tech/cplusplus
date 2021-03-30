#pragma once

#include <any>
#include <cinttypes>
#include <functional>
#include <memory>
#include <tuple>
#include <vector>

using namespace std;

namespace Framework
{
    namespace DB
    {
        class IDBConn;
        class IDBRecordSet;

        class IDBCommand
        {
        public:
            virtual ~IDBCommand(){};

            virtual void OnExecuteSql(shared_ptr<IDBConn> conn) = 0;
            virtual void OnExecuted()                           = 0;
        };

        using ExecSqlFunc = function<tuple<int32_t, string, shared_ptr<IDBRecordSet>>(shared_ptr<IDBConn> conn)>;
        using ExecSqlRecordFunc =
            function<void(int32_t errorcode, string errormsg, shared_ptr<IDBRecordSet> recordset)>;

    }  // namespace DB
}  // namespace Framework
