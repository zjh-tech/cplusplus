#include "engine/inc/mysql/mysqlmodule.h"

#include <iomanip>
#include <iostream>

#include "engine/inc/common/iocontextpool.h"
#include "engine/inc/log/env.h"
#include "engine/inc/mysql/dbcommand.h"
#include "engine/inc/mysql/mysqlconn.h"

namespace Framework
{
    namespace DB
    {
        shared_ptr<MysqlModule> GMysqlModule = nullptr;

        MysqlModule::MysqlModule() : io_worker_(io_context_)
        {
        }

        MysqlModule::~MysqlModule()
        {
            conn_vec_.clear();
            io_context_pool_ = nullptr;
        }

        bool MysqlModule::Init(vector<DBConnSpec>& conn_specs_vec, uint32_t table_max_count, uint32_t conn_max_count,
                               shared_ptr<IOContextPool> io_context_pool)
        {
            // mysql_thread_init
            // mysql_thread_end
            conn_specs_vec_  = conn_specs_vec;
            table_max_count_ = table_max_count;
            conn_max_count_  = conn_max_count;

            if (conn_specs_vec_.size() != conn_max_count)
            {
                LogErrorA("[DB] Mysql ConnMaxCount No Match");
                return false;
            }

            io_context_pool_ = io_context_pool;

            for (uint32_t i = 0; i < conn_max_count_; ++i)
            {
                DBConnPtr mysql_conn_ptr =
                    make_shared<MySQLConn>(conn_specs_vec_[i], io_context_pool_->GetNextIoContext());
                assert(mysql_conn_ptr);
                if (!mysql_conn_ptr->Connect())
                {
                    LogErrorA("[DB]  Connect {} {} Error", conn_specs_vec_[i].Host, conn_specs_vec_[i].DbName);
                    return false;
                }

                conn_vec_.push_back(mysql_conn_ptr);
            }

            return true;
        }

        bool MysqlModule::Run(uint32_t count)
        {
            if (io_context_.poll_one() == 0)
            {
                return false;
            }

            for (uint32_t i = 0; i < count; ++i)
            {
                if (io_context_.poll_one() == 0)
                {
                    break;
                }
            }

            return true;
        }

        void MysqlModule::Stop()
        {
            io_context_.stop();
            LogInfoA("[DB] Stop");
        }

        void MysqlModule::SyncExecCommand(uint64_t uid, ExecSqlFunc exec_sql_func,
                                          ExecSqlRecordFunc exec_sqlrecord_func)
        {
            DBConnPtr conn = choose_connection(uid);
            if (conn == nullptr)
            {
                LogErrorA("[DB] sync_execute choose_connection error uid = {} ", uid);
                return;
            }

            auto command = make_shared<DBCommand>(exec_sql_func, exec_sqlrecord_func);
            command->OnExecuteSql(conn);
            command->OnExecuted();
        }

        void MysqlModule::AsyncExecCommand(uint64_t uid, ExecSqlFunc exec_sql_func,
                                           ExecSqlRecordFunc exec_sqlrecord_func)
        {
            DBConnPtr conn = choose_connection(uid);
            if (conn == nullptr)
            {
                LogErrorA("[DB] async_execute choose_connection error uid = {} ", uid);
                return;
            }

            auto command = make_shared<DBCommand>(exec_sql_func, exec_sqlrecord_func);
            asio::post(conn->GetIoContext(), [this, conn, command]() {
                command->OnExecuteSql(conn);
                asio::post(io_context_, [this, command]() { command->OnExecuted(); });
            });
        }

        DBConnPtr MysqlModule::choose_connection(uint64_t uid)
        {
            return conn_vec_[HashDBIndex(uid)];
        }

        uint64_t MysqlModule::HashDBIndex(uint64_t uid)
        {
            return uid % conn_max_count_;
        }

        uint64_t MysqlModule::HashTableIndex(uint64_t uid)
        {
            return uid % table_max_count_;
        }

        string MysqlModule::GetTableNameByUID(string table_name, uint64_t uid)
        {
            uint64_t table_index = HashTableIndex(uid);
            stringstream ss;
            ss << table_name << "_" << setw(2) << setfill('0') << table_index;
            return ss.str().c_str();
        }

    }  // namespace DB
}  // namespace Framework