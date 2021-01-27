/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2019-12-02 21:48:20
 * @LastEditors  : zhengjinhong
 * @LastEditTime : 2020-01-19 11:54:33
 */

#pragma once

#include "engine/inc/config/config_interface.hpp"
#include <cinttypes>
#include <string>

namespace Framework {
namespace Config {

  template <class T1, class T2 = uint32_t>
  class CConfigTemplate : public IConfig {
  public:
    using key_type   = T2;
    using value_type = T1;

    CConfigTemplate() {
    }

    virtual ~CConfigTemplate() {
      Clear();
    }

    void Parse(const CCsvParser& parser) override {
      std::vector<uint32_t> rows         = parser.GetRows();
      uint32_t              max_row_size = rows.size();
      for (uint32_t i = 0; i < max_row_size; ++i) {
        value_type info;
        info.FillAttribute(parser, rows[i]);
        m_map[info.id] = info;
      }
    }

    void Clear() {
      m_map.clear();
    }

    void GetSize() const {
      return m_map.size();
    }

    const value_type* Find(key_type key) const {
      auto iter = m_map.find(key);
      if (iter != m_map.end()) {
        return &(iter->second);
      }

      return nullptr;
    }

    const std::map<key_type, value_type>& GetMap() const {
      return m_map;
    }

  private:
    std::map<key_type, value_type> m_map;
  };

}  // namespace Config
}  // namespace Framework
