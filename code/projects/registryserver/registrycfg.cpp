/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-04 15:50:27
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-11-13 11:16:02
 */

#include "registrycfg.h"
#include "engine/inc/common/convert.hpp"
#include "engine/inc/common/utils.hpp"
#include "tinyxml2.h"

using namespace tinyxml2;
using namespace Framework;

shared_ptr<RegistryCfg> GRegistryCfg = nullptr;

tuple<bool, uint32_t> RegistryCfg::GetServerType(string server_type_str) {
  auto iter = ServerTypeMap.find(server_type_str);
  if (iter == ServerTypeMap.end()) {
    return make_tuple(false, 0);
  }

  return make_tuple(true, iter->second);
}

tuple<shared_ptr<RegistryCfg>, ErrorStringPtr> LoadRegistryCfg(const char* path) {
  shared_ptr<RegistryCfg> cfg = make_shared<RegistryCfg>();

  XMLDocument doc;
  if (doc.LoadFile(path) != XML_SUCCESS) {
    return make_tuple(nullptr, CREATE_ERROR_STRING("LoadRegistryCfg LoadFile Error"));
  }

  XMLElement* cfgElem = doc.RootElement();
  if (cfgElem == nullptr) {
    return make_tuple(nullptr, CREATE_ERROR_STRING("LoadRegistryCfg Root Error"));
  }

  XMLElement* servertypeElem = cfgElem->FirstChildElement("servertype");
  if (servertypeElem == nullptr) {
    return make_tuple(nullptr, CREATE_ERROR_STRING("LoadRegistryCfg servertype Error"));
  }

  for (XMLElement* node = servertypeElem->FirstChildElement(); node; node = node->NextSiblingElement()) {
    string   server_type_str            = node->Attribute("type");
    uint32_t server_type                = Convert::Str2T<uint32_t>(node->Attribute("enum"));
    cfg->ServerTypeMap[server_type_str] = server_type;
  }

  XMLElement* connectionElem = cfgElem->FirstChildElement("connection");
  if (connectionElem == nullptr) {
    return make_tuple(nullptr, CREATE_ERROR_STRING("LoadRegistryCfg connection Error"));
  }

  for (XMLElement* node = connectionElem->FirstChildElement(); node; node = node->NextSiblingElement()) {
    string                start      = node->Attribute("start");
    tuple<bool, uint32_t> start_attr = cfg->GetServerType(start);
    if (get<0>(start_attr) == false) {
      return make_tuple(nullptr, CREATE_ERROR_STRING("LoadRegistryCfg start server_type Error"));
    }
    uint32_t server_id = get<1>(start_attr);

    string         end = node->Attribute("end");
    vector<string> endStrVec;
    Utils::SplitStr(end, ",", endStrVec);
    size_t           end_size = endStrVec.size();
    vector<uint32_t> endVec;
    for (size_t i = 0; i < end_size; ++i) {
      tuple<bool, uint32_t> end_attr = cfg->GetServerType(endStrVec[i]);
      if (get<0>(end_attr) == false) {
        return make_tuple(nullptr, CREATE_ERROR_STRING("LoadRegistryCfg end server_type Error"));
      }
      endVec.push_back(get<1>(end_attr));
    }

    cfg->ConntionMap[server_id] = endVec;
  }

  XMLElement* s2slistElem = cfgElem->FirstChildElement("s2slist");
  if (s2slistElem == nullptr) {
    return make_tuple(nullptr, CREATE_ERROR_STRING("LoadRegistryCfg s2slist Error"));
  }

  for (XMLElement* node = s2slistElem->FirstChildElement(); node; node = node->NextSiblingElement()) {
    auto s2sAttrPtr            = make_shared<S2SAttr>();
    s2sAttrPtr->ServerID       = Convert::Str2T<uint64_t>(node->Attribute("id"));
    s2sAttrPtr->ServerTypeStr  = node->Attribute("type");
    tuple<bool, uint32_t> attr = cfg->GetServerType(s2sAttrPtr->ServerTypeStr);
    if (get<0>(attr) == false) {
      return make_tuple(nullptr, CREATE_ERROR_STRING("LoadRegistryCfg s2slist server_type Error"));
    }
    s2sAttrPtr->ServerType = get<1>(attr);

    if (node->FindAttribute("inter")) {
      s2sAttrPtr->Inter = node->Attribute("inter");
    }

    if (node->FindAttribute("outer")) {
      s2sAttrPtr->Outer = node->Attribute("outer");
    }

    if (node->FindAttribute("token")) {
      s2sAttrPtr->Token = node->Attribute("token");
    }

    cfg->S2SAttrMap[s2sAttrPtr->ServerID] = s2sAttrPtr;
  }

  XMLElement* c2slistElem = cfgElem->FirstChildElement("c2slist");
  if (c2slistElem == nullptr) {
    return make_tuple(nullptr, CREATE_ERROR_STRING("LoadRegistryCfg c2slist Error"));
  }

  for (XMLElement* node = c2slistElem->FirstChildElement(); node; node = node->NextSiblingElement()) {
    auto c2sAttrPtr            = make_shared<C2SAttr>();
    c2sAttrPtr->ServerID       = Convert::Str2T<uint64_t>(node->Attribute("id"));
    c2sAttrPtr->ServerTypeStr  = node->Attribute("type");
    tuple<bool, uint32_t> attr = cfg->GetServerType(c2sAttrPtr->ServerTypeStr);
    if (get<0>(attr) == false) {
      return make_tuple(nullptr, CREATE_ERROR_STRING("LoadRegistryCfg c2slist server_type Error"));
    }
    c2sAttrPtr->ServerType = get<1>(attr);

    c2sAttrPtr->Inter    = node->Attribute("inter");
    c2sAttrPtr->Outer    = node->Attribute("outer");
    c2sAttrPtr->MaxCount = Convert::Str2T<uint32_t>(node->Attribute("maxcount"));

    cfg->C2SAttrMap[c2sAttrPtr->ServerID] = c2sAttrPtr;
  }

  return make_tuple(cfg, nullptr);
}
