/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-04 15:50:02
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-11-23 17:50:10
 */

#pragma once

#include "engine/inc/common/errorstring.h"
#include <cinttypes>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

using namespace std;
using namespace Framework;

struct S2SAttr {
  uint64_t ServerID      = 0;
  uint32_t ServerType    = 0;
  string   ServerTypeStr = "";
  string   Inter         = "";
  string   Outer         = "";
  string   Token         = "";
};

struct C2SAttr {
  uint64_t ServerID      = 0;
  uint32_t ServerType    = 0;
  string   ServerTypeStr = "";
  string   Inter         = "";
  string   Outer         = "";
  uint32_t MaxCount      = 0;
};

class RegistryCfg {
public:
  tuple<bool, uint32_t> GetServerType(string server_type_str);

public:
  map<string, uint32_t>              ServerTypeMap;
  map<uint32_t, vector<uint32_t>>    ConntionMap;
  map<uint64_t, shared_ptr<S2SAttr>> S2SAttrMap;
  map<uint64_t, shared_ptr<C2SAttr>> C2SAttrMap;
};

tuple<shared_ptr<RegistryCfg>, ErrorStringPtr> LoadRegistryCfg(const char* path);

extern shared_ptr<RegistryCfg> GRegistryCfg;