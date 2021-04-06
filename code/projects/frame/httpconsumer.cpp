#include "httpconsumer.h"

#include "engine/inc/log/env.h"
#include "httpclient.h"
#include "httpproducer.h"

HttpConsumer::HttpConsumer() : io_context_work(io_context)
{
}

bool HttpConsumer::Init()
{
    thread_ptr = make_shared<thread>([this]() { run(); });
    return true;
}

void HttpConsumer::UnInit()
{
    terminate = true;
    thread_ptr->join();
}

void HttpConsumer::Push(shared_ptr<HttpClient> ptr)
{
    if (!ptr)
    {
        return;
    }

    asio::post(io_context, [this, ptr]() {
        ptr->ExecHttpCmd();
        HttpProducer::Instance()->Push(ptr);
    });
}

void HttpConsumer::run()
{
    LogInfoA("Start HttpConsumer Thread");
    bool busy = false;
    while (!terminate)
    {
        busy = false;
        if (io_context.poll_one() != 0)
        {
            busy = true;
            for (uint32_t i = 0; i < 100; ++i)
            {
                if (io_context.poll_one() == 0)
                {
                    break;
                }
            }
        }

        if (!busy)
        {
            this_thread::sleep_for(chrono::milliseconds(1));
        }
    }
    LogInfoA("End HttpConsumer Thread");
}