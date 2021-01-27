/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2019-12-03 21:39:07
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-03-06 16:49:05
 */

#pragma once

#include "engine/inc/common/singleton.hpp"
#include "engine/inc/config/config.h"
#include "engine/inc/config/config_interface.hpp"
#include "engine/inc/config/config_macros.hpp"
#include "engine/inc/config/ex_config.h"
#include <vector>

namespace Framework {
namespace Config {

  class CConfigMgr : public Singleton<CConfigMgr> {
  public:
    virtual ~CConfigMgr();
    bool Load(const std::string& path);
    bool ReLoad(const std::string& path, const std::string& filename);

    IMPLEMENT_EX_CONFIG(Test);

  private:
    void SetFileProps();

  private:
    std::map<std::string, SCsvBindInfo> m_file_ptr_map;  //filename - SCsvBindInfo
  };

}  // namespace Config
}  // namespace Framework