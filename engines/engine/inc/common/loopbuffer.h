#pragma once
#include <cinttypes>
#include <cstdlib>
#include <cstring>

using namespace std;

namespace Framework
{
    class LoopBuffer
    {
    public:
        LoopBuffer(uint32_t capacity = 1024);
        ~LoopBuffer();

        void Reset();

        inline uint32_t GetCapacity() const
        {
            return capacity_;
        }

        inline uint32_t GetReadLen() const
        {
            return write_index_ - read_index_;
        }

        inline char* GetReadAddr() const
        {
            return data_ + read_index_;
        }

        inline uint32_t GetWriteLen() const
        {
            return capacity_ - write_index_;
        }

        inline char* GetWriteAddr() const
        {
            return data_ + write_index_;
        }

        inline void AddReadSize(uint32_t read_size)
        {
            read_index_ += read_size;
        }

        void Append(const void* data, uint32_t len);

        void RefreshBuffer();

    private:
        char* data_           = nullptr;
        uint32_t write_index_ = 0;
        uint32_t read_index_  = 0;
        uint32_t capacity_    = 0;
    };

}  // namespace Framework
