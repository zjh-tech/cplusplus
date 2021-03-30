
#pragma once

#include <tuple>

#include "engine/inc/mysql/idbcommand.h"

namespace Framework
{
    namespace DB
    {
        class IDBRecordSet;

        class DBCommand : public IDBCommand
        {
        public:
            DBCommand(ExecSqlFunc exec_sql_func, ExecSqlRecordFunc exec_sqlrecord_func)
            {
                exec_sql_func_       = exec_sql_func;
                exec_sqlrecord_func_ = exec_sqlrecord_func;
            }
            virtual ~DBCommand(){};

            virtual void OnExecuteSql(shared_ptr<IDBConn> conn)
            {
                recordset_ = exec_sql_func_(conn);
            }

            virtual void OnExecuted()
            {
                exec_sqlrecord_func_(get<0>(recordset_), get<1>(recordset_), get<2>(recordset_));
            }

        private:
            ExecSqlFunc exec_sql_func_ = nullptr;

            ExecSqlRecordFunc exec_sqlrecord_func_ = nullptr;

            tuple<int32_t, string, shared_ptr<IDBRecordSet>> recordset_;
        };

    }  // namespace DB
}  // namespace Framework