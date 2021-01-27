/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-04 15:27:54
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-11-16 19:09:51
 */

#include "matchserver.h"

int main(void) {
  MatchServer server;

  if (server.Init()) {
    server.Run();
  }

  server.UnInit();
  return 0;
}