#pragma once

#include <mutex>

#include "engine/inc/common/noncopyable.hpp"

namespace Framework
{
    template <typename T>
    class Singleton : Noncopyable
    {
    public:
        static T* Instance()
        {
            if (ptr_)
            {
                return ptr_;
            }

            std::call_once(once_flag_, [&]() { ptr_ = new T(); });
            return ptr_;
        }

        static void Destroy()
        {
            if (ptr_)
            {
                delete ptr_;
                ptr_ = nullptr;
            }
        }

    private:
        static std::once_flag once_flag_;
        static T* ptr_;
    };

    template <typename T>
    std::once_flag Singleton<T>::once_flag_;

    template <typename T>
    T* Singleton<T>::ptr_ = nullptr;

}  // namespace Framework
