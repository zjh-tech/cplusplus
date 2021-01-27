/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-04 11:33:05
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-12-18 11:38:30
 */

#include "servermgr.h"
#include "engine/inc/log/env.h"
#include "frame/globalset.h"
#include "frame/ssserversession.h"
#include "tsrankserver.h"
#include <memory>

using namespace std;

void ServerMgr::SetLogicServer(shared_ptr<SSServerSession> sess) {
  if (sess == nullptr) {
    return;
  }

  uint32_t server_type = sess->GetRemoteServerType();
  if (server_type == TS_RANK_SERVER) {
    sess->SetLogicServer(make_shared<TsRankServer>());
  } else {
    assert(false);
    LogWarnA("[ServerMgr] SetLogicServer Unknow ServerType={}", GetTsServerName(server_type));
  }
}