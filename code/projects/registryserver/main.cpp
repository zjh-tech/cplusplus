/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-04 15:27:54
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-01-11 14:51:32
 */

#include "registryserver.h"

int main(void) {
  RegistryServer server;

  if (server.Init()) {
    server.Run();
  }

  server.UnInit();
  return 0;
}