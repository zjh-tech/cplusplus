/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2019-12-12 17:47:20
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-03-06 16:49:45
 */
#pragma once

#include "engine/inc/common/reflector.hpp"
#include "engine/inc/config/csv_parser.hpp"
#include <map>
#include <string>
#include <vector>

namespace Framework {
namespace Config {

  class IConfig {
  public:
    virtual ~IConfig() {}

    virtual void Parse(const CCsvParser& parser) {}
    virtual void ExParse() {}

    bool Load(const std::string& file_name) {
      CCsvParser parser;
      if (parser.Load(file_name) == false) {
        return false;
      }

      Parse(parser);
      ExParse();

      return true;
    }
  };

  struct SCsvBindInfo {
    void Set(IConfig** cfg, std::string csv, std::string cls_name, bool reload) {
      pConfigAddr = cfg;
      csv_file    = csv;
      class_name  = cls_name;
      reload_flag = reload;
    }

    IConfig**   pConfigAddr = nullptr;
    std::string csv_file    = "";
    std::string class_name  = "";
    bool        reload_flag = false;
  };

}  // namespace Config
}  // namespace Framework