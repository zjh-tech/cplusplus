#include "servermgr.h"

#include "engine/inc/log/env.h"
#include "frame/globalset.h"
#include "frame/ssserversession.h"
#include "hallsession.h"
#include "loginsession.h"

void ServerMgr::SetLogicServer(shared_ptr<SSServerSession> sess)
{
    assert(sess);
    uint32_t serverType = sess->GetRemoteServerType();
    if (serverType == eServerType::LOGIN_SERVER)
    {
        sess->SetLogicServer(make_shared<LoginSession>());
    }
    else if (serverType == eServerType::HALL_SERVER)
    {
        sess->SetLogicServer(make_shared<HallSession>());
    }
    else
    {
        LogErrorA("[ServerMgr] SetLogicServer Not Find ServerType={} SSServerSession", GetServerName(serverType));
    }
}