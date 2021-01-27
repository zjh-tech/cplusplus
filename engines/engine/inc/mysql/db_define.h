/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-13 18:53:43
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-11-24 10:52:46
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

}  // namespace DB
}  // namespace Framework