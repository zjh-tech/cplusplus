/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-13 15:53:00
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-11-13 16:54:47
 */

#pragma once

#include "frame/ilogicserver.h"

class ServerMgr : public ILogicServerFactory, public enable_shared_from_this<ServerMgr> {
public:
  void SetLogicServer(shared_ptr<SSServerSession> sess) override;
};