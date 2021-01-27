/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2021-01-05 14:17:26
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-01-05 15:36:40
 */

#pragma once
#include "asio.hpp"

class AsyncModule {
public:
  AsyncModule();

  void StartReloadAllConfig(const char* path);

private:
  asio::io_context       io_context;
  asio::io_context::work io_context_work;
};