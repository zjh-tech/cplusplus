#pragma once

#include "engine/inc/common/singleton.hpp"
#include <asio.hpp>
#include <memory>
#include <thread>

using namespace Framework;
using namespace std;

class HttpClient;

//only simple use http client,large requirements use golang http replace http client pool
//can use unlock queue replace asio

class HttpConsumer : public Singleton<HttpConsumer> {
public:
  HttpConsumer();

  bool Init();

  void UnInit();

  void Push(shared_ptr<HttpClient> ptr);

private:
  void run();

private:
  shared_ptr<thread>     thread_ptr = nullptr;
  asio::io_context       io_context;
  asio::io_context::work io_context_work;
  bool                   terminate = false;
};