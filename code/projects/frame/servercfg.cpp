/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-10-15 14:33:09
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-11-12 14:51:32
 */

#include "servercfg.h"
#include "engine/inc/common/convert.hpp"
#include "engine/inc/common/utils.hpp"

using namespace Framework;

shared_ptr<ServerCfg> GServerCfg = make_shared<ServerCfg>();

tuple<bool, string, uint32_t> ConvertHostAndPort(string str) {
  vector<string> vec;
  Utils::SplitStr(str, ":", vec);
  if (vec.size() != 2) {
    return make_tuple(false, "", 0);
  }
  uint32_t port = Convert::Str2T<uint32_t>(vec[1].c_str());
  return make_tuple(true, vec[0], port);
}