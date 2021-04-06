#pragma once

#include <time.h>

#include <cstring>
#include <experimental/filesystem>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <tuple>
#include <vector>

#define MAX_PATH_SIZE 260

using namespace std;

namespace Framework
{
    class Utils
    {
    public:
        static void SplitStr(const string& src, const char* token, vector<string>& split_vec)
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
                    split_vec.emplace_back(src.substr(begin, total_len - begin));
                }
                else
                {
                    split_vec.emplace_back(src.substr(begin, end - begin));
                    begin = end + strlen(token);
                }
            }
        }

        static void SplitStr(const string& src, const char* token, vector<uint32_t>& split_vec)
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
                    split_vec.push_back(stoi(src.substr(begin, total_len - begin)));
                }
                else
                {
                    split_vec.push_back(stoi(src.substr(begin, end - begin)));
                    begin = end + strlen(token);
                }
            }
        }

        static void DeleteSubStr(string& src, const char* token)
        {
            if (src.empty())
            {
                return;
            }

            size_t token_len = strlen(token);

            size_t pos = src.find(token, 0);
            while (pos != string::npos)
            {
                src.erase(pos, pos + token_len);
                pos = src.find(token);
            }
        }

        static int32_t GetRandom(int32_t value1, int32_t value2)
        {
            int32_t min = value1;
            int32_t max = value2;
            if (value1 > value2)
            {
                min = value2;
                max = value1;
            }

            random_device rd;
            mt19937 rg(rd());
            uniform_int_distribution<int32_t> dist{min, max};
            return dist(rg);
        }

        static bool IsEqual(float a, float b)
        {
            const float floatPrecision = 0.01f;
            float value                = a - b;
            if (value > floatPrecision || value < -floatPrecision)
            {
                return false;
            }

            return true;
        }

        static uint64_t GetThreadId()
        {
            ostringstream oss;
            oss << this_thread::get_id();
            string stid            = oss.str();
            unsigned long long tid = stoull(stid);
            return tid;
        }

        static time_t GetSecTime()
        {
            return chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count();
        }

        static time_t GetMillTime()
        {
            return chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
        }

        static time_t GetMicroTime()
        {
            return chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count();
        }

        static time_t GetNanoTime()
        {
            return chrono::duration_cast<chrono::nanoseconds>(chrono::system_clock::now().time_since_epoch()).count();
        }

        static bool IsLeapYear(int year)
        {
            return (year % 100 != 0 && year % 4 == 0) || (year % 400 == 0);
        }

        static tuple<bool, uint64_t> GetTotalDayCount(int month, int year)
        {
            if (month < 1 || month > 12 || year <= 0)
            {
                return make_tuple(false, 0);
            }

            if ((month == 1) || (month == 3) || (month == 5) || (month == 7) || (month == 8) || (month == 10) || (month == 12))
            {
                return make_tuple(true, 31);
            }

            if (month == 2)
            {
                if (IsLeapYear(year))
                {
                    return make_tuple(true, 29);
                }
                else
                {
                    return make_tuple(true, 28);
                }
            }

            return make_tuple(true, 30);
        }

        //         static int StrCmp(const char* s1, const char* s2)
        //         {
        // #if defined(__linux__)
        //             return strcasecmp(s1, s2);
        // #else
        //             return stricmp(s1, s2);
        // #endif
        //         }

        //         static char* RealPath(const char* path, char* real_path)
        //         {
        // #if defined(__linux__)
        //             return realpath(path, real_path);
        // #else
        //             return _fullpath(real_path, path, MAX_PATH_SIZE);
        // #endif
        //         };

#define NEW new

#define SAFE_DELETE(x) \
    if (x)             \
    {                  \
        delete (x);    \
        (x) = NULL;    \
    }

#define ArraySize(array) (sizeof(array) / sizeof((array)[0]))
    };
}  // namespace Framework