/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-20 15:21:37
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-02-07 17:03:31
 */

#pragma once

#include "engine/inc/mysql/dbfield.h"

using namespace std;
#include <sstream>

namespace Framework {
namespace DB {

  class SQLTool {
  public:
    static string GetSelectSQL(const char* table_name, DBField* select_field, DBFieldPair* where_field) {
      stringstream db_stream;
      db_stream << "SELECT ";

      if (!select_field) {
        db_stream << " * ";
      } else {
        auto& select_vec = select_field->GetFieldVec();
        bool  first_flag = true;
        for (auto& field_name : select_vec) {
          if (!first_flag) {
            db_stream << " , ";
          }

          db_stream << field_name;
          first_flag = false;
        }
      }

      db_stream << " FROM " << table_name << " WHERE ";
      if (where_field) {
        auto& where_map  = where_field->GetFieldMap();
        bool  first_flag = true;
        for (auto iter = where_map.begin(); iter != where_map.end(); ++iter) {
          if (!first_flag) {
            db_stream << " AND ";
          }

          db_stream << iter->first << " = " << iter->second;
          first_flag = false;
        }
      }

      db_stream << ";";
      return db_stream.str();
    }

    static string GetInsertSQL(const char* table_name, DBFieldPair* insert_field) {
      stringstream db_stream;
      db_stream << "INSERT INTO " << table_name << " ( ";
      if (insert_field) {
        auto& insert_map = insert_field->GetFieldMap();
        bool  first_flag = true;
        for (auto iter = insert_map.begin(); iter != insert_map.end(); ++iter) {
          if (!first_flag) {
            db_stream << " , ";
          }

          db_stream << iter->first;
          first_flag = false;
        }

        db_stream << " ) VALUES ( ";
        first_flag = true;
        for (auto iter = insert_map.begin(); iter != insert_map.end(); ++iter) {
          if (!first_flag) {
            db_stream << " , ";
          }

          db_stream << iter->second;
          first_flag = false;
        }
      }
      db_stream << " );";
      return db_stream.str();
    }

    static string GetUpdateSQL(const char* table_name, DBFieldPair* update_field, DBFieldPair* where_field) {
      stringstream db_stream;
      db_stream << "UPDATE " << table_name << " SET ";
      if (update_field) {
        auto& update_map = update_field->GetFieldMap();
        bool  first_flag = true;
        for (auto iter = update_map.begin(); iter != update_map.end(); ++iter) {
          if (!first_flag) {
            db_stream << " , ";
          }

          db_stream << iter->first << " = " << iter->second;
          first_flag = false;
        }
      }
      if (where_field) {
        auto& where_map  = where_field->GetFieldMap();
        bool  first_flag = true;
        for (auto iter = where_map.begin(); iter != where_map.end(); ++iter) {
          if (!first_flag) {
            db_stream << " AND ";
          }

          db_stream << iter->first << " = " << iter->second;
          first_flag = false;
        }
      }
      db_stream << " ;";
      return db_stream.str();
    }

    static string GetInsertOrUpdateSQL(const char* table_name, DBFieldPair* field, DBField* key) {
      stringstream db_stream;
      db_stream << "INSERT INTO " << table_name << " ( ";
      if (field) {
        auto& map        = field->GetFieldMap();
        bool  first_flag = true;
        for (auto iter = map.begin(); iter != map.end(); ++iter) {
          if (!first_flag) {
            db_stream << " , ";
          }

          db_stream << iter->first;
          first_flag = false;
        }

        db_stream << " ) VALUES ( ";
        first_flag = true;
        for (auto iter = map.begin(); iter != map.end(); ++iter) {
          if (!first_flag) {
            db_stream << " , ";
          }

          db_stream << iter->second;
          first_flag = false;
        }
      }
      db_stream << " ) ON DUPLICATE KEY UPDATE ";

      if (key) {
        // exclude key
        auto& map        = field->GetFieldMap();
        bool  first_flag = true;
        for (auto iter = map.begin(); iter != map.end(); ++iter) {
          bool exist_flag = false;

          auto& vec = key->GetFieldVec();
          for (auto& key_str : vec) {
            if (key_str == iter->first) {
              exist_flag = true;
              break;
            }
          }

          if (exist_flag) {
            continue;
          }

          if (!first_flag) {
            db_stream << " , ";
          }

          first_flag = false;
          db_stream << iter->first << " = " << iter->second;
        }
      }
      return db_stream.str();
    }

    static string GetDeleteSQL(const char* table_name, DBFieldPair* where_field) {
      stringstream db_stream;
      db_stream << "DELETE FROM " << table_name << " WHERE ";
      if (where_field) {
        auto& where_map  = where_field->GetFieldMap();
        bool  first_flag = true;
        for (auto iter = where_map.begin(); iter != where_map.end(); ++iter) {
          if (!first_flag) {
            db_stream << " AND ";
          }

          db_stream << iter->first << " = " << iter->second;
          first_flag = false;
        }
      }
      db_stream << ";";
      return db_stream.str();
    }
  };
}  // namespace DB
}  // namespace Framework