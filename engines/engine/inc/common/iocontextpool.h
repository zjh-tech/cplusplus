#pragma once
#include <asio.hpp>
#include <memory>
#include <thread>
#include <vector>

using namespace std;

namespace Framework
{
    struct Processor
    {
        bool RunFlag_                 = true;
        shared_ptr<thread> ThreadPtr_ = nullptr;
        asio::io_context IoContext_;
        shared_ptr<asio::io_context::work> IoContextWorkPtr_ = nullptr;
    };

    class IOContextPool
    {
    public:
        void Start(uint32_t thread_num, uint32_t loop_num);

        void Stop();

        asio::io_context& GetNextIoContext();

    private:
        void run(shared_ptr<Processor> processor_ptr, uint32_t loop_num);

    private:
        vector<shared_ptr<Processor>> processor_vec_;
        size_t io_context_next_index_ = 0;
        atomic<bool> start_flag_      = false;
    };

}  // namespace Framework
