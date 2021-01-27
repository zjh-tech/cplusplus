/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-03 14:12:33
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-11-03 17:10:10
 */

#include "idmaker.h"
#include "engine/inc/common/utils.hpp"
#include <exception>
#include <sstream>

void IdMaker::Init(uint32_t server_id) {
  server_id = server_id;
}

uint64_t IdMaker::NextId() {
  unique_lock<std::mutex> lock(mutex);
  return nextId();
}

vector<uint64_t> IdMaker::NextIds(uint32_t count) {
  vector<uint64_t>        vec;
  unique_lock<std::mutex> lock(mutex);
  for (uint32_t i = 0; i < count; ++i) {
    vec.push_back(nextId());
  }
  return vec;
}

uint64_t IdMaker::nextId() {
  uint64_t now = Utils::GetMillTime();

  // 如果当前时间小于上一次ID生成的时间戳，说明系统时钟回退过这个时候应当抛出异常
  if (now < last_timestamp) {
    ostringstream s;
    s << "clock moved backwards";
    throw std::exception(std::runtime_error(s.str()));
  }

  if (last_timestamp == now) {
    // 如果是同一时间生成的，则进行毫秒内序列
    sequence = (sequence + 1) & quence_mask;
    if (0 == sequence) {
      // 毫秒内序列溢出, 阻塞到下一个毫秒,获得新的时间戳
      while (now <= last_timestamp) {
        now = Utils::GetMillTime();
      }
    }
  } else {
    sequence = 0;
  }

  last_timestamp = now;

  // 移位并通过或运算拼到一起组成64位的ID
  //1(不用) + 41(41位的时间截，可以使用69年) + 12(4095) + 10(1023) = 64位
  return ((now - twepoch) << timestamp_left_shift) | (server_id << server_id_shift) | sequence;
}
