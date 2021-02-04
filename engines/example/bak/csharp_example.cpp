/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-05-01 18:36:40
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-05-06 15:04:03
 */

//https://yizhang82.dev/hosting-coreclr

#include "engine/inc/csharp/env.h"
#include "engine/inc/log/logger.h"
#include <iostream>
#include <memory>
#include "string.h"

using namespace std;
using namespace std::experimental::filesystem::v1;

typedef char* (*bootstrap_ptr)();
typedef int (*add_ptr)(int, int);
typedef void (*set_sendmsg_ptr)(void(int, unsigned char*, int));

void SendMsg(int msgID, unsigned char* buff, int len) {
  char* temp = new char[len];
  memcpy(temp, buff, len);
  // string str(temp, len);
  // LogInfoA("SendMsg msgID = {} len ={} context={}", msgID, len, str);
  for (int i = 0; i < len; ++i) {
    std::cout << temp[i] << std::endl;
  }
}

int main(int argc, char* argv[]) {
  LOG_INIT("csharp_example", true, "debug", true);
  std::shared_ptr<CNetCore> net_core = std::make_shared<CNetCore>();
  if (net_core == nullptr) {
    return -1;
  }

  const char* pkg_path = "CSharpDll";

  if (!net_core->Init(argv[0], pkg_path)) {
    return -1;
  }

  bootstrap_ptr dele = nullptr;
  if (net_core->GetCSharpProcAddress("CSharp", "CUtil", "BootStrap", reinterpret_cast<void**>(&dele))) {
    char* msg = dele();
    LogInfoA("CUtil::BootStrap() returned {}", msg);
#if defined(__linux__)
    free(msg);
#else
    CoTaskMemFree(msg);
#endif
  }

  add_ptr add_dele = nullptr;
  if (net_core->GetCSharpProcAddress("CSharp", "CUtil", "Add", reinterpret_cast<void**>(&add_dele))) {
    int total_count = add_dele(1, 2);
    LogInfoA("CUtil::Add(1,2) = {}", total_count);
  }

  set_sendmsg_ptr set_sendmsg_dele = nullptr;
  if (net_core->GetCSharpProcAddress("CSharp", "CUtil", "SetSendMsgDele", reinterpret_cast<void**>(&set_sendmsg_dele))) {
    set_sendmsg_dele(SendMsg);
  }

  LOG_FLUSH();

  while (true) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  LOG_RELEASE();

  return 0;
}
