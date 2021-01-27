/*
 * @Author: zhengjinhong
 * @Date: 2019-11-08 16:12:31
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-01-26 10:49:59
 * @Description: file content
 */
#include "engine/inc/log/logger.h"
#if defined(__linux__)
#  include <experimental/filesystem>
#else
#  include <filesystem>
#endif
#include <fstream>
#include <iomanip>
#include <iostream>

shared_ptr<Framework::Log::Logger> GLogger = nullptr;

namespace Framework {
namespace Log {
  Logger::Logger()
    : io_worker(io_context) {
    memset(&tm_cur, 0, sizeof(tm_cur));
  }

  void Logger::Init(const string& _path, const string& _name_prefix, bool _console_flag, uint32_t _level) {
    if (is_dir_exist(_path.c_str()) == false) {
      create_dir(_path.c_str());
    }

    path         = _path;
    name_prefix  = _name_prefix;
    console_flag = _console_flag;
    if (_level >= (uint32_t)eLogLevel::DEBUG_LEVEL && _level <= (uint32_t)eLogLevel::OFF) {
      level = (eLogLevel)_level;
    }

    is_running_flag = true;
    log_thread      = thread(&Logger::run, this);
  }

  void Logger::UnInit() {
    if (!is_running_flag) {
      return;
    }

    if (stop_flag) {
      return;
    }

    stop_flag = true;
    log_thread.join();
  }

  void Logger::split(const string& src, const char* token, vector<string>& ret_vec) {
    if (src.empty()) {
      return;
    }

    size_t end       = 0;
    size_t begin     = 0;
    size_t total_len = src.length();
    while (end != string::npos) {
      end = src.find(token, begin);
      if (end == string::npos) {
        ret_vec.emplace_back(src.substr(begin, total_len - begin));
      } else {
        ret_vec.emplace_back(src.substr(begin, end - begin));
        begin = end + strlen(token);
      }
    }
  }

  void Logger::out_put(eLogLevel level, const string& content, bool async_flag) {
    int64_t   cur_mill = get_sec_time();
    struct tm tm_next;
    timestamp2tm(cur_mill, tm_next);
    if (check_diff_date(tm_next)) {
      if (cur_ofstream_ptr != nullptr) {
        cur_ofstream_ptr->close();
        cur_ofstream_ptr = nullptr;
      }

      ensure_file_exist(tm_next);
    }

    tm_cur = tm_next;

    stringstream ss;
    ss << (tm_cur.tm_year + 1900) << "-";
    ss << setw(2) << setfill('0') << (tm_cur.tm_mon + 1) << "-";
    ss << setw(2) << setfill('0') << tm_cur.tm_mday << " ";
    ss << setw(2) << setfill('0') << tm_cur.tm_hour << ":";
    ss << setw(2) << setfill('0') << tm_cur.tm_min << ":";
    ss << setw(2) << setfill('0') << tm_cur.tm_sec << " ";
    ss << GetLogLevelStr(level) << " " << content << endl;
    (*cur_ofstream_ptr) << ss.str();

    if (!async_flag) {
      cur_ofstream_ptr->flush();
      cur_ofstream_ptr->clear();
    }

#if defined(__linux__)
#else
    cout << ss.str();
#endif
  }

  bool Logger::check_diff_date(struct tm& tm_next) {
    return (tm_cur.tm_year != tm_next.tm_year) || (tm_cur.tm_mon != tm_next.tm_mon) || (tm_cur.tm_mday != tm_next.tm_mday) || (tm_cur.tm_hour != tm_next.tm_hour);
  }

  bool Logger::ensure_file_exist(struct tm& tm_next) {
    stringstream ss;
    ss << path << "/" << (tm_next.tm_year + 1900) << "_" << (tm_next.tm_mon + 1) << "_" << tm_next.tm_mday;
    if (is_dir_exist(ss.str().c_str()) == false) {
      create_dir(ss.str().c_str());
    }

    ss << "/" << (tm_next.tm_year + 1900) << "_" << (tm_next.tm_mon + 1) << "_" << tm_next.tm_mday << "_" << tm_next.tm_hour << ".log";
    cur_ofstream_ptr = make_shared<ofstream>(ss.str(), ios::out | ios::app);
    if (cur_ofstream_ptr->is_open() == false) {
      cout << "Open File:" << ss.str() << "Error" << endl;
      return false;
    }

    return true;
  }

  void Logger::timestamp2tm(time_t timestamp, struct tm& tm) {
    memset(&tm, 0, sizeof(tm));

#if defined(__linux__)
    localtime_r(&timestamp, &tm);
#else
    localtime_s(&tm, &timestamp);
#endif
  }

  bool Logger::is_dir_exist(const char* dir_path) {
#if defined(__linux__)
    return experimental::filesystem::v1::exists(dir_path);
#else
    return filesystem::exists(dir_path);
#endif
  }

  void Logger::create_dir(const char* dir_path) {
#if defined(__linux__)
    experimental::filesystem::v1::create_directories(dir_path);
#else
    filesystem::create_directories(dir_path);
#endif
  }

  void Logger::run() {
    bool busy = false;
    while (is_running_flag) {
      busy = false;
      if (io_context.poll_one() != 0) {
        busy = true;
        for (uint32_t i = 0; i < 100; ++i) {
          if (io_context.poll_one() == 0) {
            break;
          }
        }
      }

      int64_t now = get_mill_time();
      if ((flush_tick + async_flush_max_tick) <= now && cur_ofstream_ptr) {
        busy = true;
        log_mutex.lock();
        cur_ofstream_ptr->flush();
        cur_ofstream_ptr->clear();
        log_mutex.unlock();
      }

      if (stop_flag) {
        if (io_context.poll_one() == 0) {
          string content = get_content("{}#{}: [Log] Stop", __FILENAME__, __LINE__);
          out_put(eLogLevel::INFO_LEVEL, content, true);
          cur_ofstream_ptr->flush();
          cur_ofstream_ptr->clear();
          is_running_flag = false;
        }
      }

      if (!busy) {
        this_thread::sleep_for(chrono::milliseconds(1));
      }
    }
  }

}  // namespace Log
};  // namespace Framework