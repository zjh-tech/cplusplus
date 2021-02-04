/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-03-16 16:40:41
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-02-04 11:00:02
 */

#pragma once

#include <assert.h>
#include <sstream>
#include <string>

using namespace std;

namespace Framework {
class Convert {
public:
  //--------------unsafe----------
  //Template specialization ==> constexpr
  template <typename T>
  static T Str2T(const char* in) {
    if constexpr (is_same_v<bool, T>) {
      return (stoi(in) == 0) ? false : true;
    } else if constexpr (is_same_v<int32_t, T>) {
      return stoi(in);
    } else if constexpr (is_same_v<int64_t, T>) {
      return stoll(in);
    } else if constexpr (is_same_v<uint64_t, T>) {
      return stoull(in);
    } else if constexpr (is_same_v<float, T>) {
      return stof(in);
    } else if constexpr (is_same_v<double, T>) {
      return stod(in);
    } else {
      stringstream ss(in);
      T            value = 0;
      ss >> value;
      return value;
    }
  }

  //-------------safe third party use not crash-----
  template <typename T>
  static T SafeStr2T(const char* in) {
    stringstream ss(in);
    T            t = 0;
    ss >> t;
    return t;
  }

  template <typename T>
  static string T2Str(T& t) {
    if constexpr (is_same_v<const char*, T>) {
      return t;
    } else if constexpr (is_same_v<string, T>) {
      return t;
    } else {
      stringstream ss;
      ss << t;
      return ss.str();
    }
  }
};

//Template specialization ==> inline

template <>
inline bool Convert::SafeStr2T(const char* in) {
  //exclude space character
  string s(in);
  size_t index = s.find(' ', 0);
  while (index != string::npos) {
    s.erase(index, 1);
    index = s.find(' ', index);
  }

  return (s == "0") ? false : true;
}

}  // namespace Framework