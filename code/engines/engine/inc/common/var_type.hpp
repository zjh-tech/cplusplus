/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-26 11:57:26
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-11-25 13:08:18
 */

#pragma once
#include "engine/inc/common/convert.hpp"
#include <map>
#include <string>
#include <vector>

using namespace std;

namespace Framework {

class VarType {
public:
  VarType(const string& str) {
    m_value.append(str);
  };
  virtual ~VarType(){};

  operator bool() const {
    return Convert::Str2T<bool>(m_value.c_str());
  }

  operator int8_t() const {
    return Convert::Str2T<int8_t>(m_value.c_str());
  }
  operator uint8_t() const {
    return Convert::Str2T<uint8_t>(m_value.c_str());
  }
  operator int16_t() const {
    return Convert::Str2T<int16_t>(m_value.c_str());
  }
  operator uint16_t() const {
    return Convert::Str2T<uint16_t>(m_value.c_str());
  }

  operator int32_t() const {
    return Convert::Str2T<int32_t>(m_value.c_str());
  }

  operator uint32_t() const {
    return Convert::Str2T<uint32_t>(m_value.c_str());
  }

  operator int64_t() const {
    return Convert::Str2T<int64_t>(m_value.c_str());
  }

  operator uint64_t() const {
    return Convert::Str2T<uint64_t>(m_value.c_str());
  }

  operator float() const {
    return Convert::Str2T<float>(m_value.c_str());
  }

  operator double() const {
    return Convert::Str2T<double>(m_value.c_str());
  }

  operator string() const {
    return m_value;
  }

  //binary
  const char* c_str() {
    return m_value.c_str();
  }

  size_t size() const {
    return m_value.size();
  }

private:
  string m_value;
};

}  // namespace Framework