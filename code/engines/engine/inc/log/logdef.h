/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-12-17 16:07:37
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-12-17 17:59:51
 */

#pragma once

#include <cinttypes>
#include <string>

using namespace std;

namespace Framework {
namespace Log {

#ifdef WIN32
#  define __FILENAME__ (strrchr(__FILE__, '\\') + 1)
#else
#  define __FILENAME__ (strrchr(__FILE__, '/') + 1)
#endif

  enum eLogLevel : uint32_t {
    DEBUG_LEVEL = 0,
    INFO_LEVEL  = 1,
    WARN_LEVEL  = 2,
    ERROR_LEVEL = 3,
    OFF         = 4,
  };

  static const char* GLogLevelArray[] = {
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
  };

  inline const char* GetLogLevelStr(uint32_t index) {
    if (index < (sizeof(GLogLevelArray) / sizeof(GLogLevelArray)[0])) {
      return GLogLevelArray[index];
    }
    return "";
  }
}  // namespace Log
}  // namespace Framework