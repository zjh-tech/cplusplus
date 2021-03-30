#pragma once
#include <map>
#include <string>
#include <vector>

#include "engine/inc/common/convert.hpp"

using namespace std;

namespace Framework
{
    class VarType
    {
    public:
        VarType(const string& str)
        {
            value_.append(str);
        };
        virtual ~VarType(){};

        operator bool() const
        {
            return Convert::Str2T<bool>(value_.c_str());
        }

        operator int8_t() const
        {
            return Convert::Str2T<int8_t>(value_.c_str());
        }
        operator uint8_t() const
        {
            return Convert::Str2T<uint8_t>(value_.c_str());
        }
        operator int16_t() const
        {
            return Convert::Str2T<int16_t>(value_.c_str());
        }
        operator uint16_t() const
        {
            return Convert::Str2T<uint16_t>(value_.c_str());
        }

        operator int32_t() const
        {
            return Convert::Str2T<int32_t>(value_.c_str());
        }

        operator uint32_t() const
        {
            return Convert::Str2T<uint32_t>(value_.c_str());
        }

        operator int64_t() const
        {
            return Convert::Str2T<int64_t>(value_.c_str());
        }

        operator uint64_t() const
        {
            return Convert::Str2T<uint64_t>(value_.c_str());
        }

        operator float() const
        {
            return Convert::Str2T<float>(value_.c_str());
        }

        operator double() const
        {
            return Convert::Str2T<double>(value_.c_str());
        }

        operator string() const
        {
            return value_;
        }

        // binary
        const char* c_str()
        {
            return value_.c_str();
        }

        size_t size() const
        {
            return value_.size();
        }

    private:
        string value_;
    };

}  // namespace Framework