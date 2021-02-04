/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-14 14:14:17
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-02-04 18:54:16
 */

#pragma once
#include "engine/inc/mysql/db_conn.h"
#include "engine/inc/mysql/db_define.h"
#include "engine/inc/mysql/db_field.h"
#include "engine/inc/mysql/db_module.h"
#include "engine/inc/mysql/db_record_set.h"

using namespace Framework;
using namespace Framework::DB;

#define DB_INIT() \
  GMysqlModule = make_shared<MysqlModule>();

#define DB_UNINIT()         \
  if (GMysqlModule) {       \
    GMysqlModule->Stop();   \
    GMysqlModule = nullptr; \
  }
