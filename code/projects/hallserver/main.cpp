/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-04 15:27:54
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-11-16 18:52:30
 */

#include "hallserver.h"

int main(void) {
  HallServer server;

  if (server.Init()) {
    server.Run();
  }

  server.UnInit();
  return 0;
}