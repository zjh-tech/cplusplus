#include "servermgr.h"

#include "centersession.h"
#include "dbsession.h"
#include "engine/inc/log/env.h"
#include "frame/globalset.h"
#include "frame/ssserversession.h"
#include "gatewaysession.h"
#include "matchsession.h"

void ServerMgr::SetLogicServer(shared_ptr<SSServerSession> sess)
{
    assert(sess);
    uint32_t serverType = sess->GetRemoteServerType();

    if (serverType == eServerType::CENTER_SERVER)
    {
        sess->SetLogicServer(make_shared<CenterSession>());
    }
    else if (serverType == eServerType::DB_SERVER)
    {
        sess->SetLogicServer(make_shared<DBSession>());
    }
    else if (serverType == eServerType::GATEWAY_SERVER)
    {
        sess->SetLogicServer(make_shared<GatewaySession>());
    }
    else if (serverType == eServerType::MATCH_SERVER)
    {
        sess->SetLogicServer(make_shared<MatchSession>());
    }
    else
    {
        LogErrorA("[ServerMgr] SetLogicServer Not Find ServerType={} SSServerSession", GetServerName(serverType));
    }
}