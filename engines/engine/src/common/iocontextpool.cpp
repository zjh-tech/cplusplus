#include "engine/inc/common/iocontextpool.h"

#include "engine/inc/common/utils.hpp"
#include "engine/inc/log/env.h"

namespace Framework
{
    void IOContextPool::Start(uint32_t thread_num, uint32_t loop_num)
    {
        bool old_start_flag = false;
        if (start_flag_.compare_exchange_strong(old_start_flag, true) == false)
        {
            return;
        }

        for (uint32_t i = 0; i < thread_num; ++i)
        {
            auto processor               = make_shared<Processor>();
            processor->IoContextWorkPtr_ = make_shared<asio::io_context::work>(processor->IoContext_);
            processor->ThreadPtr_ = make_shared<thread>([this, processor, loop_num]() { run(processor, loop_num); });
            processor_vec_.push_back(processor);
        }
    }

    void IOContextPool::Stop()
    {
        bool old_start_flag = true;
        if (start_flag_.compare_exchange_strong(old_start_flag, false) == false)
        {
            return;
        }

        size_t total_num = processor_vec_.size();
        for (size_t i = 0; i < total_num; ++i)
        {
            processor_vec_[i]->RunFlag_ = false;
        }

        for (size_t i = 0; i < total_num; ++i)
        {
            processor_vec_[i]->ThreadPtr_->join();
        }
    }

    asio::io_context& IOContextPool::GetNextIoContext()
    {
        if (!processor_vec_.empty())
        {
            io_context_next_index_ = (io_context_next_index_ + 1) % processor_vec_.size();
            return processor_vec_[io_context_next_index_]->IoContext_;
        }
        assert(false);
        LogErrorA("IOContextPool GetNextIoContext Error");
        static asio::io_context io_context;
        static shared_ptr<asio::io_context::work> io_contextwork_ptr = make_shared<asio::io_context::work>(io_context);
        return io_context;
    }

    void IOContextPool::run(shared_ptr<Processor> processor_ptr, uint32_t loop_num)
    {
        LogInfoA("Start IoContextPool Thread ID={}", Utils::GetThreadId());
        // io_context_ptr.run();

        auto& io_context = processor_ptr->IoContext_;
        bool busy        = false;
        while (processor_ptr->RunFlag_)
        {
            busy = false;
            if (io_context.poll_one() != 0)
            {
                busy = true;
                for (uint32_t i = 0; i < loop_num; ++i)
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
        LogInfoA("Stop IoContextPool Thread ID={}", Utils::GetThreadId());
    }

}  // namespace Framework
