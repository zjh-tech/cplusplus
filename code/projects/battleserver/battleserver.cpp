
#include "battleserver.h"

#include <thread>

#include "engine/inc/log/env.h"
#include "frame/csclientsession.h"
#include "frame/httpproducer.h"
#include "frame/server.h"
#include "frame/servercfg.h"
#include "frame/servicediscoveryclient.h"
#include "frame/servicediscoveryhttpclient.h"
#include "frame/ssserversession.h"
#include "servermgr.h"
#include "staticdatamgr.h"
#include "tip.pb.h"

using namespace std;

bool BattleServer::Init()
{
    if (Server::Init() == false)
    {
        return false;
    }

    auto error_ptr = GStaticDataMgr->LoadAllCfg("../config/binary");
    if (error_ptr != nullptr)
    {
        LogErrorA("[BattleServer] LoadAllCfg Error={}", error_ptr->Str());
        return false;
    }

    GSSServerSessionMgr->Init(make_shared<ServerMgr>());

    // GServiceDiscoveryClient->Init(GServerCfg->SDConnectIp, GServerCfg->SDConnectPort, GServerCfg->ServerId, GServerCfg->Token, nullptr);
    GServiceDiscoveryHttpClient->Init(GServerCfg->SDClientUrl, GServerCfg->ServerId, GServerCfg->Token, nullptr);

    LogInfoA("[BattleServer] Init Ok");
    return true;
}

void BattleServer::Run()
{
    bool busy = false;

    auto net_module         = Net::Instance();
    auto timer_module       = TimeWheelMgr::Instance();
    auto http_client_module = HttpProducer::Instance();

    while (!IsQuit())
    {
        busy = false;

        if (net_module->Run(NET_LOOP_COUNT))
        {
            busy = true;
        }

        if (timer_module->Run(TIMER_LOOP_COUNT))
        {
            busy = true;
        }

        if (http_client_module->Run(HTTP_CLIENT_LOOP_COUNT))
        {
            busy = true;
        }

        if (!busy)
        {
            this_thread::sleep_for(chrono::milliseconds(1));
        }
    }
}
