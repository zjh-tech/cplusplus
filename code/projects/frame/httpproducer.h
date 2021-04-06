#pragma once

#include <asio.hpp>
#include <memory>
#include <thread>

#include "engine/inc/common/singleton.hpp"

using namespace Framework;
using namespace std;

class HttpClient;

class HttpProducer : public Singleton<HttpProducer>
{
public:
    HttpProducer();

    void Push(shared_ptr<HttpClient> ptr);

    bool Run(uint32_t count);

private:
    asio::io_context io_context;
    asio::io_context::work io_context_work;
    bool terminate = false;
};