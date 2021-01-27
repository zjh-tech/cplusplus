/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-13 15:53:07
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-12-18 11:41:09
 */

#include "servermgr.h"
#include "engine/inc/log/env.h"
#include "frame/globalset.h"
#include "frame/ssserversession.h"
#include "hallsession.h"

void ServerMgr::SetLogicServer(shared_ptr<SSServerSession> sess) {
  assert(sess);
  uint32_t serverType = sess->GetRemoteServerType();
  if (serverType == eServerType::HALL_SERVER) {
    sess->SetLogicServer(make_shared<HallSession>());
  } else {
    LogErrorA("[ServerMgr] SetLogicServer Not Find ServerType={} SSServerSession", GetServerName(serverType));
  }
}