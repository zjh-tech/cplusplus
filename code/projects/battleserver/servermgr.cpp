/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-13 15:53:07
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-12-18 11:37:02
 */

#include "servermgr.h"
#include "engine/inc/log/env.h"
#include "frame/globalset.h"
#include "frame/ssserversession.h"
#include "matchsession.h"

void ServerMgr::SetLogicServer(shared_ptr<SSServerSession> sess) {
  assert(sess);
  uint32_t serverType = sess->GetRemoteServerType();
  if (serverType == eServerType::MATCH_SERVER) {
    sess->SetLogicServer(make_shared<MatchSession>());
  } else {
    LogErrorA("[ServerMgr] SetLogicServer Not Find ServerType={} SSServerSession", GetServerName(serverType));
  }
}