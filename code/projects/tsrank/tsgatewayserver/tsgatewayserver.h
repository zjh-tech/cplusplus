/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-10-15 10:49:27
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-10-16 12:05:14
 */

#pragma once
#include "frame/server.h"

class TsGatewayServer : public Server {
public:
  bool Init() override;
  void Run() override;
};