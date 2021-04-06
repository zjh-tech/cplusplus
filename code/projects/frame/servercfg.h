#pragma once

#include <cinttypes>
#include <memory>
#include <string>

#include "engine/inc/common/convert.hpp"

using namespace Framework;
using namespace std;

struct ServerCfg
{
    string ServerName   = "";
    uint64_t ServerId   = 0;
    uint32_t ServerType = 0;
    string Token        = "";
    string LogDir       = "";
    uint32_t LogLevel   = 0;
    uint32_t ThreadNum  = 0;

    string SDListenIp     = "";
    uint32_t SDListenPort = 0;

    string SDConnectIp     = "";
    uint32_t SDConnectPort = 0;

    string SDClientUrl = "";

    string C2SInterListen      = "";
    string C2SOuterListen      = "";
    uint32_t C2SListenMaxCount = 0;
};

tuple<bool, string, uint32_t> ConvertHostAndPort(string str);

extern shared_ptr<ServerCfg> GServerCfg;
