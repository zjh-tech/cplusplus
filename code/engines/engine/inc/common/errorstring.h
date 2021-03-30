#pragma once

#include <cinttypes>
#include <memory>
#include <string>

using namespace std;

namespace Framework
{
    class ErrorString
    {
    public:
        ErrorString(const char* content) : content_(content)
        {
        }

        const char* Str()
        {
            return content_;
        }

    private:
        const char* content_;
    };

    using ErrorStringPtr = shared_ptr<ErrorString>;

#define CREATE_ERROR_STRING(text) make_shared<ErrorString>(text)

}  // namespace Framework