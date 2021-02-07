/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-23 18:07:16
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-02-07 17:50:52
 */

#pragma once

#include "engine/inc/common/errorstring.h"
#include "engine/inc/mysql/dbdefine.h"
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