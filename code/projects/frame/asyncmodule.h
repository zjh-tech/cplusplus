#pragma once
#include "asio.hpp"

class AsyncModule
{
public:
    AsyncModule();

    void StartReloadAllConfig(const char* path);

private:
    asio::io_context io_context;
    asio::io_context::work io_context_work;
};