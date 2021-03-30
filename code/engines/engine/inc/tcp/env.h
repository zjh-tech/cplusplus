#pragma once
#include "engine/inc/common/icoder.h"
#include "engine/inc/common/iocontextpool.h"
#include "engine/inc/common/netstream.h"
#include "engine/inc/common/singleton.hpp"
#include "engine/inc/tcp/connection.h"
#include "engine/inc/tcp/connectionmgr.h"
#include "engine/inc/tcp/listener.h"
#include "engine/inc/tcp/net.h"
#include "engine/inc/tcp/netdefine.h"

using namespace Framework;
using namespace Framework::Tcp;

#define TCP_INIT()             \
    ConnectionMgr::Instance(); \
    Net::Instance();

#define TCP_UNINIT() Net::Instance()->Stop();
