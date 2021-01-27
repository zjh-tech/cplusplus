/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-10-14 17:02:47
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-11-27 11:42:46
 */
#include "example/item.pb.h"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;
using namespace google::protobuf;

class CConfigMgr {
public:
  bool LoadAllCfg(const char* pbPath) {
    if (!loadCfg(pbPath, &item)) {
      return false;
    }

    return true;
  }

private:
  bool loadCfg(const char* pbPath, PROTOBUF_NAMESPACE_ID::Message* message) {
    ifstream     fin(pbPath);
    stringstream buffer;
    buffer << fin.rdbuf();
    message->ParseFromArray(buffer.str().c_str(), buffer.str().length());
    return true;
  }

private:
  config::itemcfg item;
};

int main(void) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  CConfigMgr cfgMgr;
  if (!cfgMgr.LoadAllCfg("D:\\ServerSet\\code\\engine\\bin\\win\\example\\config\\item.pb")) {
    std::cout << "Load Config Error" << std::endl;
    return;
  }

  std::cout << "Load Config Success" << std::endl;

  google::protobuf::ShutdownProtobufLibrary();
  return 0;
}
