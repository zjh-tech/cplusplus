/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-13 18:53:43
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-01-29 10:36:13
 */

#pragma once
#include "engine/inc/common/var_type.hpp"
#include "engine/inc/mysql/imysqlrecordset.h"
#include <map>
#include <memory>
#include <vector>

using namespace std;
using namespace Framework;

namespace Framework {
namespace DB {

  class DBRecord {
    friend class MySQLConn;

  public:
    shared_ptr<VarType> GetField(const char* field_name) {
      auto iter = m_field_map.find(field_name);
      if (iter != m_field_map.end()) {
        return make_shared<VarType>(iter->second);
      }

      return nullptr;
    }

    void AddField(const char* field_name, const char* buff, size_t len) {
      m_field_map[field_name] = string(buff, len);
    }

  private:
    map<string, string> m_field_map;
  };

  class DBRecordSet : public IMysqlRecordSet {
  public:
    friend class MySQLConn;
    const vector<DBRecordPtr>& GetDBRecordVec() override {
      return m_vec;
    }

    int64_t GetAffectRows() override {
      return m_affect_rows;
    }

    int64_t GetInsertId() override {
      return m_last_insert_id;
    }

  private:
    void AddDBRecord(shared_ptr<DBRecord> record) {
      m_vec.emplace_back(record);
    }

    void SetAffectRows(int64_t affect_rows) {
      m_affect_rows = affect_rows;
    }

    void SetLastInsertId(int64_t last_insert_id) {
      m_last_insert_id = last_insert_id;
    }

  private:
    vector<shared_ptr<DBRecord>> m_vec;
    int64_t                      m_affect_rows    = 0;
    int64_t                      m_last_insert_id = 0;
  };

}  // namespace DB
}  // namespace Framework