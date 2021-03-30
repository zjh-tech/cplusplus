#include "engine/inc/common/threadqueue.h"

namespace Framework
{
    ThreadQueue::ThreadQueue()
    {
        array_ptr_ = nullptr;
        head_      = 0;
        tail_      = 0;
        size_      = 0;
    }

    ThreadQueue::~ThreadQueue()
    {
        if (array_ptr_)
        {
            delete[] array_ptr_;
            array_ptr_ = nullptr;
        }
    }

    bool ThreadQueue::Init(int size)
    {
        if (size <= 0)
        {
            return false;
        }

        array_ptr_ = new void*[size];
        if (!array_ptr_)
        {
            return false;
        }
        size_ = size;
        return true;
    }

    bool ThreadQueue::PushBack(void* ptr)
    {
        int dist = tail_ + size_ - head_;
        int used = dist >= size_ ? (dist - size_) : dist;
        if (used >= size_ - 1) return false;

        array_ptr_[tail_] = ptr;
        if (++tail_ >= size_) tail_ = 0;

        return true;
    }

    void* ThreadQueue::PopFront()
    {
        int dist = tail_ + size_ - head_;
        int used = dist >= size_ ? (dist - size_) : dist;
        if (0 == used)
        {
            return nullptr;
        }

        void* ptr = array_ptr_[head_];
        if (++head_ >= size_) head_ = 0;

        return ptr;
    }

}  // namespace Framework
