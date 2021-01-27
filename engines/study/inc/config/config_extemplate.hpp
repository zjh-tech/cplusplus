/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-01-17 10:40:08
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-01-19 11:46:58
 */

#pragma once

#include <cinttypes>
#include <map>

namespace Framework {
namespace Config {

  template <class T1, class T2 = uint32_t>
  class CExConfigTemplate {
  public:
    using key_type   = T2;
    using value_type = T1;

    CExConfigTemplate() {
    }

    virtual ~CExConfigTemplate() {
    }

    void ClearExMap() {
      m_ex_map.clear();
    }

    void AddExAttribute(key_type key, value_type value) {
      m_ex_map[key] = value;
    }

    const value_type* FindByExkey(key_type key) const {
      auto iter = m_ex_map.find(key);
      if (iter != m_ex_map.end()) {
        return &(iter->second);
      }

      return nullptr;
    }

    const std::map<key_type, value_type>& GetExMap() const {
      return m_ex_map;
    }

  private:
    std::map<key_type, value_type> m_ex_map;
  };

}  // namespace Config
}  // namespace Framework