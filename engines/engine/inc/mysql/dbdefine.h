/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-13 18:53:43
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-02-04 14:35:16
 */

#pragma once
#include <string>

using namespace std;

namespace Framework {
namespace DB {

  struct DBConnSpec {
    string   Host = "";
    uint32_t Port = 0;
    string   DbName;
    string   User      = "";
    string   Password  = "";
    string   Character = "";
  };

#define MYSQL_ORDER_EXEC_ID 0

#define DB_EXEC_SUCCESS_CODE 0
#define DB_EXEC_FAIL_CODE 1

}  // namespace DB
}  // namespace Framework