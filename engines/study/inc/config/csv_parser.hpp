/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2019-12-03 21:33:56
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-03-17 17:05:32
 */

#pragma once

#include "engine/inc/common/utils.hpp"
#include "engine/inc/config/config_log.h"
#include <fstream>
#include <functional>
#include <map>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

namespace Framework {
namespace Config {

  class CCsvParser {
  public:
    virtual ~CCsvParser() {
      m_row2column_map.clear();
      m_column2index_map.clear();
    }

  public:
    bool Load(const std::string& file_name) {
      std::ifstream ifs;
      ifs.open(file_name);
      if (!ifs.is_open()) {
        CONFIG_LogInfoA("load open {}  error", file_name);
        return false;
      }

      //special
      uint32_t                                                    special_name_row = 1;
      std::map<uint32_t, std::function<void(const std::string&)>> m_parse_map;
      m_parse_map[special_name_row] = std::bind(&CCsvParser::parse_name_row, this, std::placeholders::_1);

      auto is_special_parse_func = [&](uint32_t row) {
        auto iter = m_parse_map.find(row);
        if (iter != m_parse_map.end()) {
          return true;
        }

        return false;
      };

      //ignore
      std::vector<uint32_t> ignore_rows;
      uint32_t              ignore_describe_row = 2;
      uint32_t              ignore_flag_row     = 3;
      uint32_t              ignore_typerow      = 4;
      ignore_rows.push_back(ignore_describe_row);
      ignore_rows.push_back(ignore_flag_row);
      ignore_rows.push_back(ignore_typerow);

      auto is_ignore_func = [&](uint32_t row) {
        auto iter = std::find(ignore_rows.begin(), ignore_rows.end(), row);
        if (iter != ignore_rows.end()) {
          return true;
        }

        return false;
      };

      uint32_t row = 0;
      while (!ifs.eof()) {
        std::string str;
        getline(ifs, str);
        if (str.empty()) {
          continue;
        }

        ++row;

        Framework::Utils::DeleteSubStr(str, " ");
        Framework::Utils::DeleteSubStr(str, "\r");
        Framework::Utils::DeleteSubStr(str, "\n");
        Framework::Utils::DeleteSubStr(str, "\t");

        if (is_ignore_func(row)) {
          continue;
        } else if (is_special_parse_func(row)) {
          m_parse_map[row](str);
        } else {
          parse_row(row, str);
        }
      }

      ifs.close();

      return true;
    }

    uint32_t GetMaxRowCount() const {
      return m_max_row_count;
    }

    std::vector<uint32_t> GetRows() const {
      std::vector<uint32_t> vec;
      for (auto iter = m_row2column_map.begin(); iter != m_row2column_map.end(); ++iter) {
        vec.push_back(iter->first);
      }

      return vec;
    }

    template <typename T>
    bool GetField(uint32_t row, const std::string& field, T& value) const {
      std::string str;
      if (!get_field(row, field, str)) {
        return false;
      }

      std::stringstream ss(str);
      ss >> value;
      return true;
    }

    bool GetField(uint32_t row, const std::string& field, bool& value) const {
      std::string str;
      if (!get_field(row, field, str)) {
        return false;
      }

      //exclude space character
      string s(str);
      size_t index = s.find(' ', 0);
      while (index != string::npos) {
        s.erase(index, 1);
        index = s.find(' ', index);
      }

      value = (s == "0") ? false : true;
      return true;
    }

  private:
    bool get_field(uint32_t row, const std::string& field, std::string& value) const {
      auto iter = m_column2index_map.find(field);
      if (iter == m_column2index_map.end()) {
        return false;
      }

      if (row > GetMaxRowCount()) {
        return false;
      }

      uint32_t column = iter->second;

      auto iter2 = m_row2column_map.find(row);
      if (iter2 == m_row2column_map.end()) {
        return false;
      }

      value = iter2->second[column];
      return true;
    }

    void parse_row(uint32_t row, const std::string& strRow) {
      std::vector<std::string> vec;
      Framework::Utils::SplitStr(strRow, ",", vec);
      m_row2column_map[row] = vec;

      if (m_max_row_count < row) {
        m_max_row_count = row;
      }
    }

    void parse_name_row(const std::string& strRow) {
      std::vector<std::string> vec;
      Framework::Utils::SplitStr(strRow, ",", vec);
      uint32_t size = vec.size();

      uint32_t column = 0;
      for (uint32_t i = 0; i < size; ++i) {
        m_column2index_map[vec[i]] = column;
        ++column;
      }
    }

  private:
    uint32_t                                     m_max_row_count = 0;
    std::map<uint32_t, std::vector<std::string>> m_row2column_map;    //row - column
    std::unordered_map<std::string, uint32_t>    m_column2index_map;  //column name - index;
  };
}  // namespace Config
}  // namespace Framework