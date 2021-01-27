/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-04 15:28:12
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-11-16 18:47:35
 */

#pragma once
#include "frame/server.h"

class HallServer : public Server {
public:
  bool Init() override;
  void Run() override;
};