/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2019-12-03 21:40:02
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-03-06 16:48:15
 */

#include "engine/inc/config/config_mgr.h"
#include "engine/inc/config/ex_config.h"
#include <sstream>

namespace Framework {
namespace Config {

  CConfigMgr::~CConfigMgr() {
    for (auto iter = m_file_ptr_map.begin(); iter != m_file_ptr_map.end(); ++iter) {
      SCsvBindInfo& bind_info = iter->second;
      if ((bind_info.pConfigAddr) && (*bind_info.pConfigAddr)) {
        delete (*bind_info.pConfigAddr);
        *bind_info.pConfigAddr = nullptr;
      }
    }
  }

  bool CConfigMgr::Load(const std::string& path) {
    SetFileProps();

    for (auto iter = m_file_ptr_map.begin(); iter != m_file_ptr_map.end(); ++iter) {
      SCsvBindInfo& bind_info = iter->second;
      if (!bind_info.pConfigAddr) {
        continue;
      }

      IConfig* config = *(bind_info.pConfigAddr);
      if (!config) {
        continue;
      }

      std::string file_name = path + bind_info.csv_file;
      if (!config->Load(file_name)) {
        return false;
      }
    }

    return true;
  }

  void CConfigMgr::SetFileProps() {
    BIND_EX_CONFIG(Test, RELOAD_FLAG);
  }

  bool CConfigMgr::ReLoad(const std::string& path, const std::string& filename) {
    auto iter = m_file_ptr_map.find(filename);
    if (iter == m_file_ptr_map.end()) {
      return false;
    }

    SCsvBindInfo& bind_info = iter->second;
    if (!bind_info.pConfigAddr) {
      return false;
    }

    auto pConfig = (IConfig*)Framework::ClassFactory::Instance()->GetClassByName(bind_info.class_name);
    if (!pConfig) {
      return false;
    }

    std::string file_name = path + bind_info.csv_file;
    if (!pConfig->Load(file_name)) {
      return false;
    }

    IConfig* config = *(bind_info.pConfigAddr);
    if (config) {
      delete config;
      config = nullptr;
    }

    *(bind_info.pConfigAddr) = pConfig;

    return true;
  }

}  // namespace Config
}  // namespace Framework