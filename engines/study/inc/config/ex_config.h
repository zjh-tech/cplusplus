/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2019-12-04 13:14:29
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-03-06 16:49:24
 */

#pragma once

#include "engine/inc/config/config.h"
#include "engine/inc/config/config_extemplate.hpp"

namespace Framework {
namespace Config {

  class CExTestAttribute {
  public:
    std::map<uint32_t, uint32_t> map;
  };

  class CExTestConfig : public CTestConfig, public CExConfigTemplate<CExTestAttribute> {
  public:
    void ExParse() override;
  };

}  // namespace Config
}  // namespace Framework