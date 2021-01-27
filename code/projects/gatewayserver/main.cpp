/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-04 15:27:54
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-01-22 14:55:52
 */

#include "gatewayserver.h"

int main(void) {
  GatewayServer server;

  if (server.Init()) {
    server.Run();
  }

  server.UnInit();
  return 0;
}
