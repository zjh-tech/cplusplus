/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-11-04 15:28:12
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-11-13 15:15:12
 */

#pragma once
#include "frame/server.h"

class GatewayServer : public Server {
public:
  bool Init() override;
  void Run() override;
};