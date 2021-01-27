/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-04 15:27:54
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-01-22 14:18:49
 */

#include "battleserver.h"

int main(void) {
  BattleServer server;

  if (server.Init()) {
    server.Run();
  }

  server.UnInit();
  return 0;
}