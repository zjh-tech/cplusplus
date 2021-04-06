#pragma once
#include "asio.hpp"
#include "engine/inc/mysql/dbdefine.h"
#include "engine/inc/mysql/idbcommand.h"

namespace Framework
{
    class IOContextPool;
}  // namespace Framework

using namespace Framework;
using namespace std;

namespace Framework
{
    namespace DB
    {
#define TABLE_NAME_MAX_LEN 256

        class IDBConn;
        using DBConnPtr = shared_ptr<IDBConn>;

        class IDBCommand;

        class MysqlModule : public enable_shared_from_this<MysqlModule>
        {
        public:
            MysqlModule();
            virtual ~MysqlModule();

        public:
            bool Init(vector<DBConnSpec>& conn_specs_vec, uint32_t table_max_count, uint32_t conn_max_count, shared_ptr<IOContextPool> processor_pool);

            bool Run(uint32_t count);

            void Stop();

            // sync
            void SyncExecCommand(uint64_t uid, ExecSqlFunc exec_sql_func, ExecSqlRecordFunc exec_sqlrecord_func);

            // async
            void AsyncExecCommand(uint64_t uid, ExecSqlFunc exec_sql_func, ExecSqlRecordFunc exec_sqlrecord_func);

            // hash
            uint64_t HashDBIndex(uint64_t uid);

            uint64_t HashTableIndex(uint64_t uid);

            string GetTableNameByUID(string table_name, uint64_t uid);

        private:
            DBConnPtr choose_connection(uint64_t uid);

        private:
            vector<DBConnSpec> conn_specs_vec_;
            uint32_t conn_max_count_  = 0;
            uint32_t table_max_count_ = 0;

            vector<DBConnPtr> conn_vec_;
            shared_ptr<IOContextPool> io_context_pool_ = nullptr;
            asio::io_context io_context_;
            asio::io_context::work io_worker_;
        };

        extern shared_ptr<MysqlModule> GMysqlModule;

    }  // namespace DB
}  // namespace Framework