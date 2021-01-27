/*
 * @Descripttion:
 * @Author: zhengjinhong
 * @Date: 2019-12-16 18:12:01
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-03-06 16:48:51
 */
#pragma once

#include "engine/inc/common/utils.hpp"
#include <cstring>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <vector>

namespace Framework {
namespace Config {

#ifdef WIN32
#  define __FILENAME__ (strrchr(__FILE__, '\\') + 1)
#else
#  define __FILENAME__ (strrchr(__FILE__, '/') + 1)
#endif

  class Logger {
  public:
    using WriteLogHandler = std::function<void(const char*)>;

    template <typename... Args>
    void info(const char* fmt, const Args&... args) {
      log(fmt, args...);
    }

    void set_write_log_handler(WriteLogHandler write_log_handler) {
      m_write_log_handler = write_log_handler;
    }

    void set_console_log(bool flag) {
      m_console_log_flag = flag;
    }

  private:
    template <typename... Args>
    void log(const char* fmt, const Args&... args) {
      std::stringstream        ss;
      std::vector<std::string> vec;
      Framework::Utils::SplitStr(fmt, "{}", vec);

      size_t index = 0;
      do_log(ss, vec, index, args...);

      size_t total_size = vec.size();
      for (; index < total_size; ++index) {
        ss << vec[index];
      }

      if (m_write_log_handler) {
        m_write_log_handler(ss.str().c_str());
      } else {
        if (m_console_log_flag) {
          std::cout << ss.str().c_str() << std::endl;
        }
      }
    }

    //C++11 variadic templates
    void do_log(std::stringstream& ss, const std::vector<std::string>& fmt_vec, size_t& index) {
    }

    template <typename T, typename... Args>
    void do_log(std::stringstream& ss, const std::vector<std::string>& fmt_vec, size_t& index, const T& first, const Args&... rest) {
      if (index > fmt_vec.size()) {
        return;
      }

      ss << fmt_vec[index];
      ss << first;
      ++index;
      do_log(ss, fmt_vec, index, rest...);
    }

  private:
    WriteLogHandler m_write_log_handler = nullptr;
    bool            m_console_log_flag  = true;
  };

  extern Logger* GLogger;

#define CONFIG_LogInfoA(fmt, ...) \
  GLogger->info("{} {}: " fmt, __FILENAME__, __LINE__, ##__VA_ARGS__);

}  // namespace Config
}  // namespace Framework