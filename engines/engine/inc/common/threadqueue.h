#pragma once

namespace Framework
{
    // unlock queue
    class ThreadQueue
    {
    public:
        ThreadQueue();
        virtual ~ThreadQueue();

    public:
        bool Init(int size);

        bool PushBack(void* ptr);

        void* PopFront();

    private:
        void** array_ptr_;
        int head_;
        int tail_;
        int size_;
    };

}  // namespace Framework