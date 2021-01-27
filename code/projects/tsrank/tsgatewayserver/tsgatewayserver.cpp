/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-10-15 10:49:34
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-11-03 19:15:44
 */

#include "tsgatewayserver.h"
#include "engine/inc/log/env.h"
#include <thread>

using namespace std;

bool TsGatewayServer::Init() {
  if (Server::Init() == false) {
    return false;
  }

  LogInfo("[TsGatewayServer] Init Ok");
  return true;
}

void TsGatewayServer::Run() {
  bool busy = false;

  while (!IsQuit()) {
    busy = false;

    if (!busy) {
      this_thread::sleep_for(chrono::milliseconds(1));
    }
  }
}