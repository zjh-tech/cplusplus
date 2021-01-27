/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-04 15:28:12
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-11-16 19:08:34
 */

#pragma once
#include "frame/server.h"

class DBServer : public Server {
public:
  bool Init() override;
  void Run() override;
  void UnInit() override;
};