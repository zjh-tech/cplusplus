/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-23 18:07:16
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-11-24 10:58:53
 */

#pragma once

#include "engine/inc/common/errorstring.h"
#include "engine/inc/mysql/db_define.h"
#include <vector>

using namespace Framework;
using namespace Framework::DB;
using namespace std;

class DatabaseCfgMgr {
public:
  ErrorStringPtr Load(const char* path);

  vector<DBConnSpec> ConnSpecs;
  uint32_t           TableMaxCount = 0;
  uint32_t           ConnMaxCount  = 0;
};

extern shared_ptr<DatabaseCfgMgr> GDatabaseCfgMgr;