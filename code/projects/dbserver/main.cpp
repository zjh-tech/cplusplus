/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-04 15:27:54
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-12-28 15:57:12
 */

#include "dbserver.h"
int main(void) {
  DBServer server;

  if (server.Init()) {
    server.Run();
  }

  server.UnInit();
  return 0;
}