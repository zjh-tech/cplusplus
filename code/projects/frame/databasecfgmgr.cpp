/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-23 18:07:30
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-11-25 11:31:25
 */
#include "databasecfgmgr.h"
#include "engine/inc/common/convert.hpp"
#include "tinyxml2.h"

using namespace Framework;
using namespace tinyxml2;

shared_ptr<DatabaseCfgMgr> GDatabaseCfgMgr = make_shared<DatabaseCfgMgr>();

ErrorStringPtr DatabaseCfgMgr::Load(const char* path) {
  XMLDocument doc;
  if (doc.LoadFile(path) != XML_SUCCESS) {
    return CREATE_ERROR_STRING("Load DatabaseCfg Error");
  }

  XMLElement* cfgElem = doc.RootElement();
  if (cfgElem == nullptr) {
    return CREATE_ERROR_STRING("Load Root Error");
  }

  TableMaxCount = Convert::Str2T<uint32_t>(cfgElem->Attribute("TableAmount"));
  for (XMLElement* node = cfgElem->FirstChildElement(); node; node = node->NextSiblingElement()) {
    DBConnSpec conn_spec;
    conn_spec.DbName    = node->Attribute("DBName");
    conn_spec.Host      = node->Attribute("DBHost");
    conn_spec.Port      = Convert::Str2T<uint32_t>(node->Attribute("DBPort"));
    conn_spec.User      = node->Attribute("DBUser");
    conn_spec.Password  = node->Attribute("DBPassword");
    conn_spec.Character = node->Attribute("Charset");
    ConnSpecs.push_back(conn_spec);
    ++ConnMaxCount;
  }

  return nullptr;
}