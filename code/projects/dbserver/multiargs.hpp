#pragma once

#include <any>
#include <vector>

using namespace std;

template <typename... Args>
class MultiArgs;

template <>
class MultiArgs<>
{
};

template <typename T, typename... Args>
class MultiArgs<T, Args...> : public MultiArgs<Args...>
{
public:
    MultiArgs(T _t, Args... _args) : MultiArgs<Args...>(_args...)
    {
        t = _t;
    }

public:
    T t;
};