#pragma once
#include "engine/inc/mysql/dbdefine.h"
#include "engine/inc/mysql/dbfield.h"
#include "engine/inc/mysql/dbrecordset.h"
#include "engine/inc/mysql/mysqlconn.h"
#include "engine/inc/mysql/mysqlmodule.h"
#include "engine/inc/mysql/sqltool.h"

using namespace Framework;
using namespace Framework::DB;

#define DB_INIT() GMysqlModule = make_shared<MysqlModule>();

#define DB_UNINIT()             \
    if (GMysqlModule)           \
    {                           \
        GMysqlModule->Stop();   \
        GMysqlModule = nullptr; \
    }
