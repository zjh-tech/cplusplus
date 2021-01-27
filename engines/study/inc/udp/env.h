/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-13 11:28:43
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-04-14 14:54:03
 */

#pragma once
#include "engine/inc/common/net/interface.h"
#include "engine/inc/common/net/main_module.h"
#include "engine/inc/common/net/net_stream.h"
#include "engine/inc/common/net/option.h"
#include "engine/inc/common/net/processor_pool.h"
#include "engine/inc/common/singleton.hpp"
#include "engine/inc/udp/kcp_mgr.h"
#include "engine/inc/udp/session.h"
#include "engine/inc/udp/session_create_factory.h"
#include "engine/inc/udp/session_mgr.h"
#include "engine/inc/udp/socket.h"
#include "engine/inc/udp/udp_client.h"
#include "engine/inc/udp/udp_server.h"
#include <asio.hpp>

using namespace Framework;
using namespace Framework::Udp;

namespace Framework {
namespace Udp {

#define UDP_INIT()                   \
  CSessionCreateFactory::Instance(); \
  CSessionMgr::Instance();

#define UDP_RELEASE()                           \
  CSessionCreateFactory::Instance()->Destroy(); \
  CSessionMgr::Instance()->Destroy();

}  // namespace Udp
}  // namespace Framework