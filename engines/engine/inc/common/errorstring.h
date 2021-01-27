/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-19 17:14:16
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-11-23 17:55:21
 */

#pragma once

#include <cinttypes>
#include <memory>
#include <string>

using namespace std;

namespace Framework {

class ErrorString {
public:
  ErrorString(const char* text)
    : s(text) {
  }

  const char* Str() {
    return s;
  }

private:
  const char* s;
};

using ErrorStringPtr = shared_ptr<ErrorString>;

#define CREATE_ERROR_STRING(text) \
  make_shared<ErrorString>(text)

}  // namespace Framework