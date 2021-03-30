#include "engine/inc/log/logger.h"

#include <assert.h>

#include <experimental/filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>

shared_ptr<Framework::Log::Logger> GLogger = nullptr;

namespace Framework
{
    namespace Log
    {
        Logger::Logger()
        {
            last_flush_tick_    = std::time(nullptr);
            evt_vector_ptr_     = new vector<LogEvent>();
            evt_two_vector_ptr_ = new vector<LogEvent>();
        }

        Logger::~Logger()
        {
            if (evt_vector_ptr_)
            {
                delete (evt_vector_ptr_);
                evt_vector_ptr_ = nullptr;
            }

            if (evt_two_vector_ptr_)
            {
                delete (evt_two_vector_ptr_);
                evt_two_vector_ptr_ = nullptr;
            }
        }

        void Logger::Init(const string& path, const string& name_prefix, uint32_t level)
        {
            if (experimental::filesystem::v1::exists(path.c_str()) == false)
            {
                experimental::filesystem::v1::create_directories(path.c_str());
            }

            path_        = path;
            name_prefix_ = name_prefix;
            if (level >= (uint32_t)eLogLevel::DEBUG_LEVEL && level <= (uint32_t)eLogLevel::OFF)
            {
                level_ = (eLogLevel)level;
            }

            is_running_flag_ = true;
            log_thread_      = thread(&Logger::run, this);
        }

        void Logger::UnInit()
        {
            if (!is_running_flag_)
            {
                return;
            }

            if (stop_flag_)
            {
                return;
            }

            stop_flag_ = true;
            log_thread_.join();
        }

        void Logger::split(const string& src, const char* token, vector<string>& ret_vec)
        {
            if (src.empty())
            {
                return;
            }

            size_t end       = 0;
            size_t begin     = 0;
            size_t total_len = src.length();
            while (end != string::npos)
            {
                end = src.find(token, begin);
                if (end == string::npos)
                {
                    ret_vec.emplace_back(src.substr(begin, total_len - begin));
                }
                else
                {
                    ret_vec.emplace_back(src.substr(begin, end - begin));
                    begin = end + strlen(token);
                }
            }
        }

        string Logger::get_prefix(time_t now)
        {
            stringstream ss;
            std::tm* tm_cur_ptr = std::localtime(&now);
            ss << (tm_cur_ptr->tm_year + 1900) << "-";
            ss << setw(2) << setfill('0') << (tm_cur_ptr->tm_mon + 1) << "-";
            ss << setw(2) << setfill('0') << tm_cur_ptr->tm_mday << " ";
            ss << setw(2) << setfill('0') << tm_cur_ptr->tm_hour << ":";
            ss << setw(2) << setfill('0') << tm_cur_ptr->tm_min << ":";
            ss << setw(2) << setfill('0') << tm_cur_ptr->tm_sec << " ";
            return ss.str();
        }

        void Logger::out_put(const string& prefix, eLogLevel level, const string& content)
        {
            has_flush_datas_flag_ = true;
            (*ofstream_ptr_) << prefix;
            (*ofstream_ptr_) << GetLogLevelStr(level) << " " << content << endl;
        }

        void Logger::ensure_file_exist(time_t now)
        {
            bool create_file_flag = false;

            std::tm* tm_cur_ptr = std::localtime(&now);

            if (last_sec_ == 0)
            {
                create_file_flag = true;
            }
            else
            {
                std::tm* tm_last_ptr = std::localtime(&last_sec_);
                create_file_flag =
                    (tm_last_ptr->tm_year != tm_cur_ptr->tm_year) || (tm_last_ptr->tm_mon != tm_cur_ptr->tm_mon) ||
                    (tm_last_ptr->tm_mday != tm_cur_ptr->tm_mday) || (tm_last_ptr->tm_hour != tm_cur_ptr->tm_hour);
            }

            if (create_file_flag == false)
            {
                return;
            }

            last_sec_ = now;

            if (ofstream_ptr_ != nullptr)
            {
                ofstream_ptr_->flush();
                ofstream_ptr_->clear();
                ofstream_ptr_->close();
                ofstream_ptr_ = nullptr;
            }

            stringstream ss;
            ss << path_ << "/" << (tm_cur_ptr->tm_year + 1900) << "_" << (tm_cur_ptr->tm_mon + 1) << "_"
               << tm_cur_ptr->tm_mday;
            if (experimental::filesystem::v1::exists(ss.str().c_str()) == false)
            {
                experimental::filesystem::v1::create_directories(ss.str().c_str());
            }

            ss << "/" << (tm_cur_ptr->tm_year + 1900) << "_" << (tm_cur_ptr->tm_mon + 1) << "_" << tm_cur_ptr->tm_mday
               << "_" << tm_cur_ptr->tm_hour << ".log";

            ofstream_ptr_ = make_shared<ofstream>(ss.str(), ios::out | ios::app);
            if (ofstream_ptr_ == nullptr)
            {
                return;
            }
            if (ofstream_ptr_->is_open() == false)
            {
                cout << "Open File:" << ss.str() << "Error" << endl;
                return;
            }

            return;
        }

        void Logger::run()
        {
            bool busy = false;
            while (is_running_flag_)
            {
                busy = false;

                {
                    lock_guard<mutex> lock_guard(log_vector_mutex_);
                    vector<LogEvent>* temp_ptr = evt_two_vector_ptr_;
                    evt_two_vector_ptr_        = evt_vector_ptr_;
                    evt_vector_ptr_            = temp_ptr;
                    evt_vector_ptr_->clear();
                }

                std::time_t cur_sec = std::time(nullptr);
                ensure_file_exist(cur_sec);
                string prefix_str = get_prefix(cur_sec);
                for (auto& evt : *evt_two_vector_ptr_)
                {
                    out_put(prefix_str, evt.Level_, evt.Content_);
                    ++qps_count;
                }

                if (ofstream_ptr_ && has_flush_datas_flag_)
                {
                    std::time_t now = std::time(nullptr);
                    if ((last_flush_tick_ + async_flush_max_tick_) <= now ||
                        evt_two_vector_ptr_->size() >= max_write_count)
                    {
                        busy = true;
                        ofstream_ptr_->flush();
                        ofstream_ptr_->clear();

                        last_flush_tick_      = now;
                        has_flush_datas_flag_ = false;

                        if (qps_count == qps_loop_count)
                        {
                            qps_end_tick = chrono::duration_cast<chrono::milliseconds>(
                                               chrono::system_clock::now().time_since_epoch())
                                               .count();
                        }
                    }
                }

                if (stop_flag_)
                {
                    lock_guard<mutex> lock_guard(log_vector_mutex_);
                    if (evt_two_vector_ptr_->empty() && evt_vector_ptr_->empty())
                    {
                        ofstream_ptr_->flush();
                        ofstream_ptr_->clear();
                        ofstream_ptr_->close();
                        ofstream_ptr_    = nullptr;
                        is_running_flag_ = false;
                    }
                }

                if (!busy)
                {
                    this_thread::sleep_for(chrono::milliseconds(1));
                }
            }
        }

    }  // namespace Log
}  // namespace Framework
