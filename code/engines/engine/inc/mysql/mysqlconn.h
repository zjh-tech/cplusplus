#pragma once

#include <mysql.h>

#include <list>

#include "engine/inc/mysql/dbdefine.h"
#include "engine/inc/mysql/idbconn.h"

using namespace std;

namespace Framework
{
    namespace DB
    {
        class IDBRecordSet;

        class MySQLConn : public enable_shared_from_this<MySQLConn>, public IDBConn
        {
        public:
            MySQLConn(DBConnSpec& db_conn_sepc, asio::io_context& io_context);
            virtual ~MySQLConn();

        public:
            bool Connect() override;
            asio::io_context& GetIoContext() override
            {
                return io_context_;
            }

            string EscapeString(const string& from) override;
            shared_ptr<IDBRecordSet> ExecuteSql(const string& sql) override;
            uint32_t GetErrorCode() override;
            string GetErrorMsg() override;

            void BeginTransact() override;
            void CommitTransact() override;
            void RollbackTransact() override;

        private:
            bool is_connected();
            bool check_connection();
            void release();

        private:
            DBConnSpec db_conn_sepc_;

            asio::io_context& io_context_;

            volatile bool connected_flag_ = false;

            MYSQL* mysql_ptr = nullptr;
        };

    }  // namespace DB
}  // namespace Framework