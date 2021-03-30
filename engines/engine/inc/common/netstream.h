
#pragma once
#include <cassert>
#include <cinttypes>
#include <cstring>
#include <string>

namespace Framework
{
    class NetStream
    {
    public:
        NetStream();
        NetStream(const char* buff, size_t size);
        NetStream(const NetStream& r);
        virtual ~NetStream();

    public:
        const char* Data();
        size_t Size();

        void Reset();

        void EnsureCapacity(size_t len);

        static uint16_t n2hs(uint16_t value);
        static uint16_t h2ns(uint16_t value);
        static uint32_t h2nl(uint32_t val);
        static uint32_t n2hl(uint32_t val);
        static uint64_t h2nll(uint64_t val);
        static uint64_t n2hll(uint64_t val);

        template <typename T>
        T Read()
        {
            if constexpr (std::is_same_v<bool, T>)
            {
                // uint8_t bool same
                assert((rpos_ + sizeof(uint8_t)) <= wpos_);
                uint8_t value = *(reinterpret_cast<uint8_t const*>(buff_ + rpos_));
                rpos_ += sizeof(uint8_t);
                return (bool)value;
            }
            else if constexpr (std::is_same_v<uint8_t, T> || std::is_same_v<int8_t, T>)
            {
                assert((rpos_ + sizeof(T)) <= wpos_);
                T value = *(reinterpret_cast<T const*>(buff_ + rpos_));
                rpos_ += sizeof(T);
                return value;
            }
            else if constexpr (std::is_same_v<uint16_t, T> || std::is_same_v<int16_t, T>)
            {
                assert((rpos_ + sizeof(T)) <= wpos_);
                T value = n2hs(*(reinterpret_cast<T const*>(buff_ + rpos_)));
                rpos_ += sizeof(T);
                return value;
            }
            else if constexpr (std::is_same_v<uint32_t, T> || std::is_same_v<int32_t, T>)
            {
                assert((rpos_ + sizeof(T)) <= wpos_);
                T value = n2hl(*(reinterpret_cast<T const*>(buff_ + rpos_)));
                rpos_ += sizeof(T);
                return value;
            }
            else if constexpr (std::is_same_v<uint64_t, T> || std::is_same_v<int64_t, T>)
            {
                assert((rpos_ + sizeof(T)) <= wpos_);
                T value = n2hll(*(reinterpret_cast<T const*>(buff_ + rpos_)));
                rpos_ += sizeof(T);
                return value;
            }
            else if constexpr (std::is_same_v<std::string, T>)
            {
                assert((rpos_ + sizeof(uint16_t)) <= wpos_);
                uint16_t len = n2hs(*(reinterpret_cast<uint16_t const*>(buff_ + rpos_)));
                rpos_ += sizeof(uint16_t);
                assert((rpos_ + len) <= wpos_);
                size_t pos = rpos_;
                rpos_ += sizeof(len);
                return std::string(buff_ + pos, len);
            }
            else
            {
                assert(false);
                return T();
            }
        }

        template <typename T>
        NetStream* Write(T& value)
        {
            if constexpr (std::is_same_v<bool, T>)
            {
                // uint8_t bool same
                EnsureCapacity(sizeof(uint8_t));
                *(uint8_t*)(buff_ + wpos_) = (uint8_t)value;
                wpos_ += sizeof(uint8_t);
            }
            else if constexpr (std::is_same_v<uint8_t, T> || std::is_same_v<int8_t, T>)
            {
                EnsureCapacity(sizeof(uint8_t));
                *(T*)(buff_ + wpos_) = value;
                wpos_ += sizeof(uint8_t);
            }
            else if constexpr (std::is_same_v<uint16_t, T> || std::is_same_v<int16_t, T>)
            {
                EnsureCapacity(sizeof(uint16_t));
                *(T*)(buff_ + wpos_) = h2ns(value);
                wpos_ += sizeof(uint16_t);
            }
            else if constexpr (std::is_same_v<uint32_t, T> || std::is_same_v<int32_t, T>)
            {
                EnsureCapacity(sizeof(uint32_t));
                *(T*)(buff_ + wpos_) = h2nl(value);
                wpos_ += sizeof(uint32_t);
            }
            else if constexpr (std::is_same_v<uint64_t, T> || std::is_same_v<int64_t, T>)
            {
                EnsureCapacity(sizeof(uint64_t));
                *(T*)(buff_ + wpos_) = h2nll(value);
                wpos_ += sizeof(uint64_t);
            }
            else if constexpr (std::is_same_v<std::string, T>)
            {
                size_t len = value.size();
                EnsureCapacity(sizeof(uint16_t) + len);
                *(uint16_t*)(buff_ + wpos_) = h2ns(value);
                wpos_ += sizeof(uint16_t);
                memcpy(&buff_[wpos_], value.str(), len);
                wpos_ += len;
            }
            else
            {
                assert(false);
            }

            return this;
        }

        template <typename T>
        NetStream& operator>>(T& value)
        {
            value = Read<T>();
            return *this;
        }

        template <typename T>
        NetStream& operator<<(T& value)
        {
            return *(Write<T>(value));
        }

        void ReadBuff(char* buff, size_t len);

        void WriteBuff(const char* buff, size_t len);

        size_t GetRPos();

        size_t GetWPos();

    private:
        char* buff_      = nullptr;
        size_t capacity_ = 0;
        size_t rpos_     = 0;
        size_t wpos_     = 0;

        static const size_t s_default_size = 64;
    };

}  // namespace Framework