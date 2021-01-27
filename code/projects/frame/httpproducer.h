/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2021-01-19 10:55:59
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-01-26 10:09:18
 */
#pragma once

#include "engine/inc/common/singleton.hpp"
#include <asio.hpp>
#include <memory>
#include <thread>

using namespace Framework;
using namespace std;

class HttpClient;

class HttpProducer : public Singleton<HttpProducer> {
public:
  HttpProducer();

  void Push(shared_ptr<HttpClient> ptr);

  bool Run(uint32_t count);

private:
  asio::io_context       io_context;
  asio::io_context::work io_context_work;
  bool                   terminate = false;
};