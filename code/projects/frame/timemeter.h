/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-03 10:47:31
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-11-03 11:54:22
 */

#pragma once
#include <cinttypes>
#include <sstream>
#include <vector>

using namespace std;

class TimeMeter {
public:
  TimeMeter(int64_t limit_sec);

  void Stamp();

  void Reset();

  void CheckOut(const char* file_name, int line);

private:
  int64_t limit_mill_sec   = 0;
  int64_t start_stamp_tick = 0;

  size_t          index = 0;
  vector<int64_t> stamp_vec;
  stringstream    buf;
};