
#pragma once

#include "engine/inc/common/errorstring.h"

using namespace Framework;

class FrameCfgMgr
{
public:
    static ErrorStringPtr LoadServerCfg(const char* path);
};