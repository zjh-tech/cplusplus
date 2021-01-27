/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-03 14:12:26
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-11-03 17:10:32
 */

#pragma once

#include "engine/inc/common/singleton.hpp"
#include <mutex>
#include <vector>

using namespace std;
using namespace Framework;

class IdMaker : public Singleton<IdMaker> {
public:
  void Init(uint32_t server_id);

  uint64_t NextId();

  vector<uint64_t> NextIds(uint32_t count);

private:
  uint64_t nextId();

private:
  std::mutex mutex;

  //开始时间截 (2018-01-01 00:00:00.000)
  const uint64_t twepoch = 1514736000000;

  //机器id所占的位数
  const uint32_t server_id_bits = 12;

  //序列所占的位数
  const uint32_t sequence_bits = 10;

  //机器ID向左移12位
  const uint32_t server_id_shift = sequence_bits;

  //时间截向左移22位
  const uint32_t timestamp_left_shift = sequence_bits + server_id_bits;

  const uint32_t max_server_id = -1 ^ (-1 << server_id_bits);

  const uint32_t quence_mask = -1 ^ (-1 << sequence_bits);

  //0-4095
  uint32_t server_id = 0;
  //0-1023
  uint32_t sequence = 0;

  //上次生成ID的时间截
  uint64_t last_timestamp = 0;
};
