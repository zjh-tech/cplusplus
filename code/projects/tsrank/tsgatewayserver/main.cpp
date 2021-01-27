/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-10-15 10:49:04
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-10-15 19:31:03
 */

#include "tsgatewayserver.h"

int main(void) {
  TsGatewayServer server;

  if (server.Init()) {
    server.Run();
  }

  server.UnInit();
  return 0;
}