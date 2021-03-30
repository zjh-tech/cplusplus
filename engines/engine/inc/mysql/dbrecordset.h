#pragma once
#include <map>
#include <memory>
#include <vector>

#include "engine/inc/common/var_type.hpp"
#include "engine/inc/mysql/idbrecordset.h"

using namespace std;
using namespace Framework;

namespace Framework
{
    namespace DB
    {
        class DBRecord
        {
        public:
            friend class MySQLConn;

            shared_ptr<VarType> GetField(const char* field_name)
            {
                auto iter = field_map_.find(field_name);
                if (iter != field_map_.end())
                {
                    return make_shared<VarType>(iter->second);
                }

                return nullptr;
            }

        private:
            void AddField(const char* field_name, const char* buff, size_t len)
            {
                field_map_[field_name] = string(buff, len);
            }

        private:
            map<string, string> field_map_;
        };

        class DBRecordSet : public IDBRecordSet
        {
        public:
            friend class MySQLConn;

            const vector<DBRecordPtr>* GetDBRecordVec() override
            {
                return &vec_;
            }

            int64_t GetAffectRows() override
            {
                return affect_rows_;
            }

            int64_t GetInsertId() override
            {
                return last_insert_id_;
            }

        private:
            void AddDBRecord(shared_ptr<DBRecord> record)
            {
                vec_.emplace_back(record);
            }

            void SetAffectRows(int64_t affect_rows)
            {
                affect_rows_ = affect_rows;
            }

            void SetLastInsertId(int64_t last_insert_id)
            {
                last_insert_id_ = last_insert_id;
            }

        private:
            vector<shared_ptr<DBRecord>> vec_;
            int64_t affect_rows_    = 0;
            int64_t last_insert_id_ = 0;
        };

    }  // namespace DB
}  // namespace Framework