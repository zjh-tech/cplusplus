/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-03-16 17:23:16
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-03-17 17:59:10
 */

#include "engine/inc/common/convert.hpp"
#include <iostream>

using namespace Framework;

int main(void) {
  bool flag = CConvert::str2t<bool>("1");

  int8_t   value1 = CConvert::str2t<int8_t>("1");
  uint8_t  value2 = CConvert::str2t<uint8_t>("1");
  int16_t  value3 = CConvert::str2t<int16_t>("1");
  uint16_t value4 = CConvert::str2t<uint16_t>("1");
  int32_t  value5 = CConvert::str2t<int32_t>("1");
  uint32_t value6 = CConvert::str2t<uint32_t>("1");
  int64_t  value7 = CConvert::str2t<int64_t>("1");
  uint64_t value8 = CConvert::str2t<uint64_t>("1");

  //bool un_safe_value = CConvert::str2t<bool>("aaa");
  //int32_t un_safe_value1 = CConvert::str2t<int32_t>("aaa");

  int32_t safe_value  = CConvert::safe_str2t<int8_t>("aaa");
  bool    safe_value1 = CConvert::safe_str2t<bool>("aaa");
  bool    safe_value2 = CConvert::safe_str2t<bool>("1  ");

  //--------------------------------------------
  bool   temp_flag = true;
  string strb      = CConvert::str2t(temp_flag);

  int8_t temp_value1 = 1;
  string str1        = CConvert::str2t(temp_value1);

  uint8_t temp_value2 = 1;
  string  str2        = CConvert::str2t(temp_value2);

  int16_t temp_value3 = 1;
  string  str3        = CConvert::str2t(temp_value3);

  uint16_t temp_value4 = 1;
  string   str4        = CConvert::str2t(temp_value4);

  int32_t temp_value5 = 1;
  string  str5        = CConvert::str2t(temp_value5);

  uint32_t temp_value6 = 1;
  string   str6        = CConvert::str2t(temp_value6);

  int64_t temp_value7 = 1;
  string  str7        = CConvert::str2t(temp_value7);

  uint64_t temp_value8 = 1;
  string   str8        = CConvert::str2t(temp_value8);

  std::cout << flag << value1 << value2 << value3 << value4 << value5 << value6 << value7 << value8 << safe_value << safe_value1 << safe_value2 << std::endl;

  return 0;
}