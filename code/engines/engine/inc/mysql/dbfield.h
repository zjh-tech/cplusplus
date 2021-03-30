#pragma once
#include <map>
#include <string>
#include <vector>

#include "engine/inc/common/convert.hpp"

using namespace Framework;
using namespace std;

namespace Framework
{
    namespace DB
    {
        class DBFieldPair
        {
        public:
            virtual ~DBFieldPair(){};

        public:
            template <typename T>
            void AddField(const char* field_name, T& value)
            {
                stringstream ss;

                if constexpr (is_same_v<const char*, T>)
                {
                    string escape_str = escape_string(value);
                    ss << "'";
                    ss << escape_str;
                    ss << "'";
                }
                else if constexpr (is_same_v<char*, T>)
                {
                    string escape_str = escape_string(value);
                    ss << "'";
                    ss << escape_str;
                    ss << "'";
                }
                else if constexpr (is_same_v<string, T>)
                {
                    string escape_str = escape_string(value);
                    ss << "'";
                    ss << escape_str;
                    ss << "'";
                }
                else if constexpr (is_same_v<const string, T>)
                {
                    string escape_str = escape_string(value);
                    ss << "'";
                    ss << escape_str;
                    ss << "'";
                }
                else
                {
                    ss << value;
                }

                field_map_[field_name] = ss.str();
            }

            inline map<string, string>& GetFieldMap()
            {
                return field_map_;
            }

        private:
            string escape_string(const string& from)
            {
                string to;
                to.resize(from.size() * 2 + 1);
                size_t index          = 0;
                size_t from_len       = from.size();
                const char* from_data = from.c_str();
                for (size_t i = 0; i < from_len; ++i)
                {
                    char c = from_data[i];
                    switch (c)
                    {
                        case '\0':
                            to[index++] = '\\';
                            to[index++] = '0';
                            break;
                        case '\n':
                            to[index++] = '\\';
                            to[index++] = 'n';
                            break;
                        case '\r':
                            to[index++] = '\\';
                            to[index++] = 'r';
                            break;
                        case 0x1a:
                            to[index++] = '\\';
                            to[index++] = 'Z';
                            break;
                        case '\'':
                            to[index++] = '\\';
                            to[index++] = '\'';
                            break;
                        case '"':
                            to[index++] = '\\';
                            to[index++] = '\"';
                            break;
                        case '\\':
                            to[index++] = '\\';
                            to[index++] = '\\';
                            break;
                        default:
                            to[index++] = c;
                    }
                }
                to.resize(index);
                return to;
            }

        private:
            map<string, string> field_map_;
        };

        class DBField
        {
        public:
            friend class SQLTool;

            virtual ~DBField(){};

        public:
            void AddField(const char* field_name)
            {
                field_vec_.push_back(field_name);
            }

        private:
            vector<string>& GetFieldVec()
            {
                return field_vec_;
            }

        private:
            vector<string> field_vec_;
        };

    }  // namespace DB
}  // namespace Framework