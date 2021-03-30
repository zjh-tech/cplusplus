#include "engine/inc/common/netstream.h"

#include <arpa/inet.h>

#include <cstdlib>

using namespace std;

namespace Framework
{
    uint16_t NetStream::n2hs(uint16_t value)
    {
        return ntohs(value);
    }

    uint16_t NetStream::h2ns(uint16_t value)
    {
        return htons(value);
    }

    uint32_t NetStream::h2nl(uint32_t val)
    {
        return htonl(val);
    }

    uint32_t NetStream::n2hl(uint32_t val)
    {
        return ntohl(val);
    }

    uint64_t NetStream::h2nll(uint64_t val)
    {
        return (((uint64_t)htonl(val)) << 32) + htonl(val >> 32);
    }

    uint64_t NetStream::n2hll(uint64_t val)
    {
        return (((uint64_t)ntohl(val)) << 32) + ntohl(val >> 32);
    }

    NetStream::NetStream()
    {
        buff_     = static_cast<char*>(malloc(s_default_size));
        capacity_ = s_default_size;
    }

    NetStream::NetStream(const NetStream& r)
    {
        buff_     = static_cast<char*>(malloc(r.capacity_));
        capacity_ = r.capacity_;
        memcpy(&buff_[wpos_], &r.buff_, capacity_);
        rpos_ = r.rpos_;
        wpos_ = r.wpos_;
    }

    NetStream::NetStream(const char* buff, size_t size)
    {
        assert(size != 0);
        buff_     = static_cast<char*>(malloc(size));
        capacity_ = size;
        memcpy(&buff_[wpos_], buff, size);
        wpos_ += size;
    }

    NetStream::~NetStream()
    {
        if (buff_ != nullptr)
        {
            free(buff_);
            buff_ = nullptr;
        }
    }

    const char* NetStream::Data()
    {
        return buff_;
    }

    size_t NetStream::Size()
    {
        return wpos_;
    }

    void NetStream::Reset()
    {
        rpos_ = 0;
        wpos_ = 0;
    }

    void NetStream::EnsureCapacity(size_t len)
    {
        if (capacity_ < (wpos_ + len))
        {
            buff_     = static_cast<char*>(realloc(buff_, capacity_ + len + s_default_size));
            capacity_ = capacity_ + len + s_default_size;
        }
    }

    void NetStream::ReadBuff(char* buff, size_t len)
    {
        assert(buff);
        assert((rpos_ + len) <= wpos_);

        memcpy(buff, &buff_[rpos_], len);
        rpos_ += len;
    }

    void NetStream::WriteBuff(const char* buff, size_t len)
    {
        EnsureCapacity(len);
        memcpy(&buff_[wpos_], buff, len);
        wpos_ += len;
    }

    size_t NetStream::GetRPos()
    {
        return rpos_;
    }

    size_t NetStream::GetWPos()
    {
        return wpos_;
    }

}  // namespace Framework