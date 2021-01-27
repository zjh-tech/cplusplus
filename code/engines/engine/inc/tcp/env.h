/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-13 11:17:18
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-01-08 16:26:57
 */
#pragma once
#include "engine/inc/common/icoder.h"
#include "engine/inc/common/iocontextpool.h"
#include "engine/inc/common/net_stream.h"
#include "engine/inc/common/singleton.hpp"
#include "engine/inc/tcp/connection.h"
#include "engine/inc/tcp/connectionmgr.h"
#include "engine/inc/tcp/listener.h"
#include "engine/inc/tcp/net.h"
#include "engine/inc/tcp/option.h"

using namespace Framework;
using namespace Framework::Tcp;

#define TCP_INIT()           \
  ConnectionMgr::Instance(); \
  Net::Instance();

#define TCP_UNINIT() \
  Net::Instance()->Stop();
