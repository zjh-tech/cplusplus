/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-04 15:27:54
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-11-16 19:03:10
 */

#include "centerserver.h"

int main(void) {
  CenterServer server;

  if (server.Init()) {
    server.Run();
  }

  server.UnInit();
  return 0;
}