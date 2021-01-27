/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-04 11:32:59
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-11-04 11:39:07
 */

#pragma once
#include "engine/inc/common/singleton.hpp"
#include "frame/ilogicserver.h"

using namespace Framework;

class ServerMgr : public Singleton<ServerMgr>, public ILogicServerFactory {
public:
  void SetLogicServer(shared_ptr<SSServerSession> sess) override;
};