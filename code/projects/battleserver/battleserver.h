/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-04 15:28:12
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-01-07 14:33:45
 */

#pragma once
#include "frame/server.h"

class BattleServer : public Server {
public:
  bool Init() override;
  void Run() override;
};