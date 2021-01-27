/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-10-15 16:05:42
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-01-25 17:09:04
 */

#include "framecfgmgr.h"
#include "servercfg.h"
#include "tinyxml2.h"

using namespace tinyxml2;

ErrorStringPtr FrameCfgMgr::LoadServerCfg(const char* path) {
  XMLDocument doc;
  if (doc.LoadFile(path) != XML_SUCCESS) {
    return CREATE_ERROR_STRING("ReadServerCfg LoadFile Error");
  }

  XMLElement* cfgElem = doc.RootElement();
  if (cfgElem == nullptr) {
    return CREATE_ERROR_STRING("ReadServerCfg Root Error");
  }

  XMLElement* serverNameElem = cfgElem->FirstChildElement("servicename");
  if (serverNameElem == nullptr) {
    return CREATE_ERROR_STRING("ReadServerCfg servicename Error");
  }
  GServerCfg->ServerName = serverNameElem->GetText();

  XMLElement* serverTypeElem = cfgElem->FirstChildElement("servertype");
  if (serverTypeElem == nullptr) {
    return CREATE_ERROR_STRING("ReadServerCfg servertype Error");
  }
  GServerCfg->ServerType = Convert::Str2T<uint64_t>(serverTypeElem->GetText());

  XMLElement* serverIdElem = cfgElem->FirstChildElement("serverid");
  if (serverIdElem == nullptr) {
    return CREATE_ERROR_STRING("ReadServerCfg serverid Error");
  }
  GServerCfg->ServerId = Convert::Str2T<uint64_t>(serverIdElem->GetText());

  XMLElement* tokenElem = cfgElem->FirstChildElement("token");
  if (tokenElem != nullptr) {
    //RegistryServer Not Token
    GServerCfg->Token = tokenElem->GetText();
  }

  XMLElement* logDirElem = cfgElem->FirstChildElement("logdir");
  if (logDirElem == nullptr) {
    return CREATE_ERROR_STRING("ReadServerCfg logdir Error");
  }
  GServerCfg->LogDir = logDirElem->GetText();

  XMLElement* logLevelElem = cfgElem->FirstChildElement("loglevel");
  if (logLevelElem == nullptr) {
    return CREATE_ERROR_STRING("ReadServerCfg loglevel Error");
  }
  GServerCfg->LogLevel = Convert::Str2T<uint32_t>(logLevelElem->GetText());

  XMLElement* threadnumElem = cfgElem->FirstChildElement("threadnum");
  if (threadnumElem == nullptr) {
    return CREATE_ERROR_STRING("ReadServerCfg threadnum Error");
  }
  GServerCfg->ThreadNum = Convert::Str2T<uint32_t>(threadnumElem->GetText());

  XMLElement* sdlistenipElem = cfgElem->FirstChildElement("sdlistenip");
  if (sdlistenipElem != nullptr) {
    GServerCfg->SDListenIp = sdlistenipElem->GetText();
  }

  XMLElement* sdlistenportElem = cfgElem->FirstChildElement("sdlistenport");
  if (sdlistenportElem != nullptr) {
    GServerCfg->SDListenPort = Convert::Str2T<uint32_t>(sdlistenportElem->GetText());
  }

  XMLElement* sdclienturlElem = cfgElem->FirstChildElement("sdclient_url");
  if (sdclienturlElem != nullptr) {
    GServerCfg->SDClientUrl = sdclienturlElem->GetText();
  }

  XMLElement* sdconnectipElem = cfgElem->FirstChildElement("sdconnectip");
  if (sdconnectipElem != nullptr) {
    GServerCfg->SDConnectIp = sdconnectipElem->GetText();
  }

  XMLElement* sdconnectportElem = cfgElem->FirstChildElement("sdconnectport");
  if (sdconnectportElem != nullptr) {
    GServerCfg->SDConnectPort = Convert::Str2T<uint32_t>(sdconnectportElem->GetText());
  }

  return nullptr;
}