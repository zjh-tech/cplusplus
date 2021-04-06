#pragma once

#include <cinttypes>

#include "engine/inc/common/utils.hpp"

enum eMsgCode : uint32_t
{
    SUCCESS = 0,
    FAIL    = 1,
};

//---------------------------
enum eServerType : uint32_t
{
    UNKNOW_SERVER   = 0,
    REGISTER_SERVER = 1,
    LOGIN_SERVER    = 2,
    GATEWAY_SERVER  = 3,
    CENTER_SERVER   = 4,
    MATCH_SERVER    = 5,
    HALL_SERVER     = 6,
    BATTLE_SERVER   = 7,
    DB_SERVER       = 8,
};

static const char* g_szServerName[] = {
    "Unknow", "RegisterServer", "LoginServer", "GatewayServer", "CenterServer", "MatchServer", "HallServer", "BattleServer", "DbServer",
};

inline const char* GetServerName(uint32_t wServerType)
{
    if (wServerType >= ArraySize(g_szServerName))
    {
        return "";
    }
    return g_szServerName[wServerType];
}

//-----------Ts----------------
enum eTsServerType : uint32_t
{
    TS_GATEWAY_SERVER = 1,
    TS_RANK_SERVER    = 2,
};

static const char* g_szTsServerName[] = {
    "Unknow",
    "TsGateway",
    "TsRank",
};

inline const char* GetTsServerName(uint32_t wServerType)
{
    if (wServerType >= ArraySize(g_szTsServerName))
    {
        return "";
    }
    return g_szTsServerName[wServerType];
}
