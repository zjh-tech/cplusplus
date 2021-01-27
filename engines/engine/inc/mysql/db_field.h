/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-15 10:07:36
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-11-25 11:44:09
 */

#pragma once
#include "engine/inc/common/convert.hpp"
#include <map>
#include <string>
#include <vector>

using namespace Framework;
using namespace std;

namespace Framework {
namespace DB {

  class DBFieldPair {
  public:
    friend class SQLTool;

    virtual ~DBFieldPair(){};

  public:
    template <typename T>
    void AddField(const char* field_name, T& value) {
      string value_str        = Convert::Str2T(value);
      m_field_map[field_name] = value_str;
    }

  private:
    map<string, string>& GetFieldMap() {
      return m_field_map;
    }

  private:
    map<string, string> m_field_map;
  };

  class DBField {
  public:
    friend class SQLTool;

    virtual ~DBField(){};

  public:
    template <typename T>
    void AddField(const char* field_name) {
      m_field_vec.push_back(field_name);
    }

  private:
    vector<string>& GetFieldVec() {
      return m_field_vec;
    }

  private:
    vector<string> m_field_vec;
  };

}  // namespace DB
}  // namespace Framework