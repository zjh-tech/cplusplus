#pragma once
#include <stdio.h>
#include <time.h>

#include <atomic>
#include <cstring>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <sstream>
#include <thread>
#include <vector>

#include "engine/inc/log/logdefine.h"

using namespace std;

namespace Framework
{
    namespace Log
    {
        class Logger
        {
        public:
            Logger();
            ~Logger();

            void Init(const string& path, const string& name_prefix, uint32_t level);

            void UnInit();

            template <typename... Args>
            void AddLogEvent(eLogLevel level, const char* fmt, const Args&... args)
            {
                if (level_ < level)
                {
                    return;
                }

                if (qps_count == 0)
                {
                    qps_start_tick =
                        chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch())
                            .count();
                }

                if (stop_flag_)
                {
                    return;
                }

                string content = get_content(fmt, args...);
                // string content = std::format(fmt,args...);  C++20

                lock_guard<mutex> lock_guard(log_vector_mutex_);
                evt_vector_ptr_->emplace_back(LogEvent(level, content));
            }

        private:
            template <typename... Args>
            string get_content(const char* fmt, const Args&... args)
            {
                stringstream ss;
                vector<string> vec;
                split(fmt, "{}", vec);

                size_t index = 0;
                do_log(ss, vec, index, args...);

                size_t total_size = vec.size();
                for (; index < total_size; ++index)
                {
                    ss << vec[index];
                }
                return ss.str();
            }

            // C++11 variadic templates
            void do_log(stringstream& ss, const vector<string>& fmt_vec, size_t& index)
            {
            }

            template <typename T, typename... Args>
            void do_log(stringstream& ss, const vector<string>& fmt_vec, size_t& index, const T& first,
                        const Args&... rest)
            {
                if (index > fmt_vec.size())
                {
                    return;
                }

                ss << fmt_vec[index];
                ss << first;
                ++index;
                do_log(ss, fmt_vec, index, rest...);
            }

            void split(const string& src, const char* token, vector<string>& ret_vec);

            string get_prefix(time_t now);

            void out_put(const string& prefix, eLogLevel level, const string& content);

            void ensure_file_exist(time_t now);

            void run();

        private:
            string path_;
            string name_prefix_;
            eLogLevel level_ = eLogLevel::INFO_LEVEL;

            thread log_thread_;
            bool is_running_flag_ = false;
            bool stop_flag_       = false;

            std::time_t last_sec_                 = 0;
            shared_ptr<ofstream> ofstream_ptr_    = nullptr;
            bool has_flush_datas_flag_            = false;
            std::time_t last_flush_tick_          = 0;
            vector<LogEvent>* evt_vector_ptr_     = nullptr;
            vector<LogEvent>* evt_two_vector_ptr_ = nullptr;
            mutex log_vector_mutex_;

            std::time_t async_flush_max_tick_ = 1;
            uint32_t max_write_count          = 1000;

        public:
            int64_t qps_count      = 0;
            int64_t qps_loop_count = 1;
            int64_t qps_start_tick = 0;
            int64_t qps_end_tick   = 0;
        };
    }  // namespace Log
};     // namespace Framework

extern shared_ptr<Framework::Log::Logger> GLogger;
