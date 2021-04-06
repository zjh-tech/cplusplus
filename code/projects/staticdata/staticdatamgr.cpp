#include "staticdatamgr.h"
#include "engine/inc/common/utils.hpp"
#include "engine/inc/log/env.h"
#include "google/protobuf/message.h"
#include "tip.pb.h"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace Framework;

shared_ptr<StaticDataMgr> GStaticDataMgr = make_shared<StaticDataMgr>();

StaticDataMgr::~StaticDataMgr() {
  SAFE_DELETE(tip_ptr);
}

bool StaticDataMgr::load_cfg(const char* path, google::protobuf::Message* msg) {
  load_path = path;
  ifstream fs(path);
  if (!fs.is_open()) {
    return false;
  }
  stringstream buffer;
  buffer << fs.rdbuf();
  return msg->ParseFromArray(buffer.str().c_str(), buffer.str().length());
}

const char* StaticDataMgr::GetPath() {
  return load_path;
}

ErrorStringPtr StaticDataMgr::LoadAllCfg(const char* path) {
  tip_ptr = NEW config::tipcfg();
  string        prefix_path(path);
  string        tip_path = prefix_path + "/tip.pb";
  if (!load_cfg(tip_path.c_str(), tip_ptr)) {
    return CREATE_ERROR_STRING("load tip  file error..");
  }
  LogInfoA("[StaticDataMgr] TipCfg={}", tip_ptr->DebugString());

  return nullptr;
}

const config::tipcfg* StaticDataMgr::GetTipCfg() {
  return tip_ptr;
}

const config::tip* StaticDataMgr::GetTipByID(uint32_t id) {
  if (tip_ptr) {
    auto iter = tip_ptr->datas().find(id);
    if (iter != tip_ptr->datas().end()) {
      return &(iter->second);
    }
  }
  return nullptr;
}
