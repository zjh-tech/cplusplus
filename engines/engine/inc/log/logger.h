/*
 * @Author: zhengjinhong
 * @Date: 2019-08-19 11:27:24
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-02-07 17:17:40
 * @Description: logger
 */

#pragma once
#include "asio.hpp"
#include "engine/inc/log/logdefine.h"
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <sstream>
#include <stdio.h>
#include <thread>
#include <time.h>
#include <vector>

using namespace std;

namespace Framework {
namespace Log {
  class Logger {
  public:
    Logger();

    void Init(const string& _path, const string& _name_prefix, bool _console_flag, uint32_t _level);

    void UnInit();

    template <typename... Args>
    void AddLogEvent(eLogLevel _level, bool async_flag, const char* _fmt, const Args&... _args) {
      if (_level < level) {
        return;
      }

      if (stop_flag) {
        return;
      }

      string content = get_content(_fmt, _args...);
      log_mutex.lock();
      if (async_flag) {
        asio::post(io_context, [this, _level, content]() {
          out_put(_level, content, true);
        });
      } else {
        out_put(_level, content, false);
      }
      log_mutex.unlock();
    }

  private:
    template <typename... Args>
    string get_content(const char* fmt, const Args&... args) {
      stringstream   ss;
      vector<string> vec;
      split(fmt, "{}", vec);

      size_t index = 0;
      do_log(ss, vec, index, args...);

      size_t total_size = vec.size();
      for (; index < total_size; ++index) {
        ss << vec[index];
      }
      return ss.str();
    }

    //C++11 variadic templates
    void do_log(stringstream& ss, const vector<string>& fmt_vec, size_t& index) {
    }

    template <typename T, typename... Args>
    void do_log(stringstream& ss, const vector<string>& fmt_vec, size_t& index, const T& first, const Args&... rest) {
      if (index > fmt_vec.size()) {
        return;
      }

      ss << fmt_vec[index];
      ss << first;
      ++index;
      do_log(ss, fmt_vec, index, rest...);
    }

    void split(const string& src, const char* token, vector<string>& ret_vec);

    void out_put(eLogLevel level, const string& content, bool async_flag);

    inline int64_t get_sec_time() {
      return chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count();
    }

    inline int64_t get_mill_time() {
      return chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count();
    }

    void timestamp2tm(time_t timestamp, struct tm& tm);

    bool check_diff_date(struct tm& tm_next);

    bool ensure_file_exist(struct tm& tm_next);

    bool is_dir_exist(const char* dir_path);

    void create_dir(const char* dir_path);

    void run();

  private:
    string                 path;
    string                 name_prefix;
    bool                   console_flag = false;
    eLogLevel              level        = eLogLevel::INFO_LEVEL;
    mutex                  log_mutex;
    thread                 log_thread;
    asio::io_context       io_context;
    asio::io_context::work io_worker;
    bool                   is_running_flag = false;
    struct tm              tm_cur;
    shared_ptr<ofstream>   cur_ofstream_ptr;
    int64_t                flush_tick           = 0;
    int64_t                async_flush_max_tick = 100;
    bool                   stop_flag            = false;
  };
}  // namespace Log
};  // namespace Framework

extern shared_ptr<Framework::Log::Logger> GLogger;
