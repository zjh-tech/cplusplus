

#include "engine/inc/mysql/mysqlconn.h"

#include "engine/inc/log/env.h"
#include "engine/inc/mysql/dbrecordset.h"

namespace Framework
{
    namespace DB
    {
        MySQLConn::MySQLConn(DBConnSpec& db_conn_sepc, asio::io_context& io_context)
            : db_conn_sepc_(db_conn_sepc), io_context_(io_context)
        {
        }

        MySQLConn::~MySQLConn()
        {
            release();
        }

        uint32_t MySQLConn::GetErrorCode()
        {
            return mysql_errno(mysql_ptr);
        }

        std::string MySQLConn::GetErrorMsg()
        {
            return mysql_error(mysql_ptr);
        }

        bool MySQLConn::Connect()
        {
            if (connected_flag_)
            {
                return true;
            }

            if (!mysql_ptr)
            {
                mysql_ptr = mysql_init(nullptr);
            }

            if (!mysql_ptr)
            {
                return false;
            }

            if (!mysql_real_connect(mysql_ptr, db_conn_sepc_.Host.c_str(), db_conn_sepc_.User.c_str(),
                                    db_conn_sepc_.Password.c_str(), db_conn_sepc_.DbName.c_str(), db_conn_sepc_.Port,
                                    nullptr, 0))
            {
                release();
                LogErrorA("mysql_real_connect failed {} , {},{},{}", db_conn_sepc_.Host.c_str(),
                          db_conn_sepc_.DbName.c_str(), GetErrorCode(), GetErrorMsg());
                return false;
            }

            connected_flag_ = true;

            mysql_set_character_set(mysql_ptr, db_conn_sepc_.Character.c_str());

            char reconnect = 1;
            mysql_options(mysql_ptr, MYSQL_OPT_RECONNECT, &reconnect);

            return true;
        }

        void MySQLConn::release()
        {
            if (mysql_ptr)
            {
                mysql_close(mysql_ptr);
                mysql_ptr = nullptr;
            }

            connected_flag_ = false;
            LogInfoA("[db] ip: {}  session {} close", db_conn_sepc_.Host.c_str(), db_conn_sepc_.DbName.c_str());
        }

        bool MySQLConn::is_connected()
        {
            return connected_flag_ && (mysql_ptr != nullptr);
        }

        string MySQLConn::EscapeString(const string& from)
        {
            string to;
            to.resize(from.size() * 2 + 1);
            size_t index = mysql_real_escape_string(mysql_ptr, to.data(), from.c_str(), from.size());
            to.resize(index);
            return to;
        }

        shared_ptr<IDBRecordSet> MySQLConn::ExecuteSql(const string& sql)
        {
            shared_ptr<DBRecordSet> record_set_ptr = nullptr;
            if (!check_connection())
            {
                return record_set_ptr;
            }

            LogInfoA("[db] ExecuteSql {}", sql);

            // contain binary data must use mysql_real_query
            int code = mysql_real_query(mysql_ptr, sql.c_str(), (unsigned long)sql.size());
            if (code != 0)
            {
                LogErrorA("[db] mysql_real_query {} error {} {} ", sql.c_str(), GetErrorCode(), GetErrorMsg());
                return record_set_ptr;
            }

            MYSQL_RES* mysql_res = mysql_store_result(mysql_ptr);
            if (mysql_res == nullptr)
            {
                int64_t affect_rows = mysql_affected_rows(mysql_ptr);
                int64_t insert_id   = mysql_insert_id(mysql_ptr);
                record_set_ptr      = make_shared<DBRecordSet>();
                assert(record_set_ptr);

                record_set_ptr = make_shared<DBRecordSet>();
                assert(record_set_ptr);
                record_set_ptr->SetAffectRows(affect_rows);
                record_set_ptr->SetLastInsertId(insert_id);
            }
            else
            {
                // select sql
                record_set_ptr = make_shared<DBRecordSet>();
                assert(record_set_ptr);
                unsigned int field_count = mysql_num_fields(mysql_res);
                MYSQL_FIELD* fields      = mysql_fetch_fields(mysql_res);

                MYSQL_ROW mysql_row = nullptr;
                while ((mysql_row = mysql_fetch_row(mysql_res)))
                {
                    unsigned long* lengths = mysql_fetch_lengths(mysql_res);

                    auto record_ptr = make_shared<DBRecord>();
                    for (unsigned int i = 0; i < field_count; ++i)
                    {
                        record_ptr->AddField(fields[i].name, mysql_row[i], lengths[i]);
                    }
                    record_set_ptr->AddDBRecord(record_ptr);
                }

                mysql_free_result(mysql_res);
                mysql_res = nullptr;
            }

            return record_set_ptr;
        }

        void MySQLConn::BeginTransact()
        {
            mysql_autocommit(mysql_ptr, 0);
        }

        void MySQLConn::CommitTransact()
        {
            mysql_commit(mysql_ptr);
            mysql_autocommit(mysql_ptr, 1);
        }

        void MySQLConn::RollbackTransact()
        {
            mysql_rollback(mysql_ptr);
            mysql_autocommit(mysql_ptr, 1);
        }

        bool MySQLConn::check_connection()
        {
            if (connected_flag_ == false && mysql_ptr != nullptr)
            {
                release();
            }

            if (mysql_ptr == nullptr)
            {
                connected_flag_ = Connect();
            }

            return connected_flag_;
        }

    }  // namespace DB
}  // namespace Framework