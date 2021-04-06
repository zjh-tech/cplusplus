#pragma once

#include "frame/ilogicserver.h"

class ServerMgr : public ILogicServerFactory, public enable_shared_from_this<ServerMgr>
{
public:
    void SetLogicServer(shared_ptr<SSServerSession> sess) override;
};