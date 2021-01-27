/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2019-12-04 13:19:55
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-03-06 16:48:22
 */

#include "engine/inc/config/ex_config.h"
#include "engine/inc/common/utils.hpp"
#include <vector>

namespace Framework {
namespace Config {

  REGISTER_REFLECTOR(CExTestConfig);

  void CExTestConfig::ExParse() {
    auto& map = GetMap();
    for (auto iter = map.begin(); iter != map.end(); ++iter) {
      auto&            info = iter->second;
      CExTestAttribute ex_info;

      std::vector<std::string> one_vec;
      Framework::Utils::SplitStr(info.effect, "|", one_vec);

      for (auto& one_info : one_vec) {
        std::vector<uint32_t> two_vec;
        Framework::Utils::SplitStr(one_info, "_", two_vec);
        if (two_vec.size() == 2) {
          ex_info.map[two_vec[0]] = two_vec[1];
        }
      }

      AddExAttribute(iter->first, ex_info);
    }
  }

}  // namespace Config
}  // namespace Framework