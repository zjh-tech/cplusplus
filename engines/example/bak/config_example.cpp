/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2019-12-06 13:41:10
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-03-17 18:52:02
 */

#include "engine/inc/config/config_log.h"
#include "engine/inc/config/config_mgr.h"
#include <iostream>

using Framework::Config::CConfigMgr;
using Framework::Config::Logger;

void write_log(const char* log) {
  //write in file
  std::cout << log << std::endl;
}

int main(void) {
  Framework::Config::GLogger = new Framework::Config::Logger();
  Framework::Config::GLogger->set_write_log_handler(write_log);

  CConfigMgr::Instance()->Load("config/csv/");
  auto pInfo = CConfigMgr::Instance()->GetExTestConfig()->Find(101);
  if (pInfo) {
    std::cout << pInfo->effect << std::endl;
  }

  CConfigMgr::Instance()->ReLoad("config/csv/", "Test");
  auto pNewInfo = CConfigMgr::Instance()->GetExTestConfig()->Find(101);
  if (pNewInfo) {
    std::cout << pNewInfo->effect << std::endl;
  }

  return 0;
}
