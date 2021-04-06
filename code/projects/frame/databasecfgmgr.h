
#pragma once

#include <vector>

#include "engine/inc/common/errorstring.h"
#include "engine/inc/mysql/dbdefine.h"

using namespace Framework;
using namespace Framework::DB;
using namespace std;

class DatabaseCfgMgr
{
public:
    ErrorStringPtr Load(const char* path);

    vector<DBConnSpec> ConnSpecs;
    uint32_t TableMaxCount = 0;
    uint32_t ConnMaxCount  = 0;
};

extern shared_ptr<DatabaseCfgMgr> GDatabaseCfgMgr;