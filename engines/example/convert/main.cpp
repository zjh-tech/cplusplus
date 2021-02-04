/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-03-16 17:23:16
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-02-04 19:06:50
 */

#include "engine/inc/common/convert.hpp"
#include <iostream>

using namespace Framework;
using namespace std;

int main(void) {
  bool flag = Convert::Str2T<bool>("1");

  int8_t   value1 = Convert::Str2T<int8_t>("1");
  uint8_t  value2 = Convert::Str2T<uint8_t>("1");
  int16_t  value3 = Convert::Str2T<int16_t>("1");
  uint16_t value4 = Convert::Str2T<uint16_t>("1");
  int32_t  value5 = Convert::Str2T<int32_t>("1");
  uint32_t value6 = Convert::Str2T<uint32_t>("1");
  int64_t  value7 = Convert::Str2T<int64_t>("1");
  uint64_t value8 = Convert::Str2T<uint64_t>("1");

  //bool    un_safe_value  = Convert::Str2T<bool>("aaa");
  //int32_t un_safe_value1 = Convert::Str2T<int32_t>("aaa");

  int32_t safe_value  = Convert::SafeStr2T<int8_t>("aaa");
  bool    safe_value1 = Convert::SafeStr2T<bool>("aaa");
  bool    safe_value2 = Convert::SafeStr2T<bool>("1  ");

  //--------------------------------------------
  bool   temp_flag = true;
  string strb      = Convert::T2Str(temp_flag);

  int8_t temp_value1 = 1;
  string str1        = Convert::T2Str(temp_value1);

  uint8_t temp_value2 = 1;
  string  str2        = Convert::T2Str(temp_value2);

  int16_t temp_value3 = 1;
  string  str3        = Convert::T2Str(temp_value3);

  uint16_t temp_value4 = 1;
  string   str4        = Convert::T2Str(temp_value4);

  int32_t temp_value5 = 1;
  string  str5        = Convert::T2Str(temp_value5);

  uint32_t temp_value6 = 1;
  string   str6        = Convert::T2Str(temp_value6);

  int64_t temp_value7 = 1;
  string  str7        = Convert::T2Str(temp_value7);

  uint64_t temp_value8 = 1;
  string   str8        = Convert::T2Str(temp_value8);

  cout << flag << value1 << value2 << value3 << value4 << value5 << value6 << value7 << value8 << safe_value << safe_value1 << safe_value2 << endl;

  return 0;
}