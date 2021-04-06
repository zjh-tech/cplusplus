#include "gatewayserver.h"

#include <thread>

#include "clientsession.h"
#include "engine/inc/log/env.h"
#include "frame/coder.h"
#include "frame/csclientsession.h"
#include "frame/httpproducer.h"
#include "frame/server.h"
#include "frame/servercfg.h"
#include "frame/servicediscoveryclient.h"
#include "frame/servicediscoveryhttpclient.h"
#include "frame/ssserversession.h"
#include "servermgr.h"

using namespace std;

bool GatewayServer::Init()
{
    if (Server::Init() == false)
    {
        return false;
    }

    GSSServerSessionMgr->Init(make_shared<ServerMgr>());

    // GServiceDiscoveryClient->Init(GServerCfg->SDConnectIp, GServerCfg->SDConnectPort, GServerCfg->ServerId, GServerCfg->Token, []() {
    GServiceDiscoveryHttpClient->Init(GServerCfg->SDClientUrl, GServerCfg->ServerId, GServerCfg->Token, []() {
        if (GServerCfg->C2SInterListen == "" || GServerCfg->C2SOuterListen == "")
        {
            LogErrorA("[GatewayServer] C2SInterListen Error");
            GServer->Quit();
            return;
        }

        tuple<bool, string, uint32_t> attr = ConvertHostAndPort(GServerCfg->C2SInterListen);
        if (get<0>(attr) == false)
        {
            LogInfoA("[GatewayServer] C2SInterListen ConvertHostAndPort  Error");
            GServer->Quit();
            return;
        }

        GCSClientSessionMgr->Listen(get<1>(attr), get<2>(attr), make_shared<ClientSession>(), make_shared<Coder>(), GServer->GetIOContextPool());
    });

    LogInfoA("[GatewayServer] Init Ok");
    return true;
}

void GatewayServer::Run()
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