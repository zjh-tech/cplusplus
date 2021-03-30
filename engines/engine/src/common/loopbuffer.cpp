#include "engine/inc/common/loopbuffer.h"

namespace Framework
{
    LoopBuffer::LoopBuffer(uint32_t capacity) : write_index_(0), read_index_(0), capacity_(capacity)
    {
        data_ = static_cast<char*>(std::malloc(capacity_));
    }

    LoopBuffer::~LoopBuffer()
    {
        free(data_);
        data_ = nullptr;
    }

    void LoopBuffer::Reset()
    {
        write_index_ = 0;
        read_index_  = 0;
    }

    void LoopBuffer::Append(const void* data, uint32_t len)
    {
        uint32_t write_size = GetWriteLen();
        if (len > write_size)
        {
            uint32_t need_size = len - write_size;
            data_              = static_cast<char*>(realloc(data_, capacity_ + need_size));
            capacity_ += need_size;
        }

        std::memcpy(data_ + write_index_, data, len);
        write_index_ += len;
    }

    void LoopBuffer::RefreshBuffer()
    {
        if (read_index_ == 0)
        {
            return;
        }

        uint32_t read_len = GetReadLen();
        if (read_len != 0)
        {
            memmove(data_, GetReadAddr(), read_len);
        }
        read_index_  = 0;
        write_index_ = read_len;
    }

}  // namespace Framework