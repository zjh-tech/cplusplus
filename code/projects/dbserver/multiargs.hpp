/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-12-28 11:12:43
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-12-28 14:00:55
 */

#pragma once

#include <any>
#include <vector>

using namespace std;

template <typename... Args>
class MultiArgs;

template <>
class MultiArgs<> {};

template <typename T, typename... Args>
class MultiArgs<T, Args...> : public MultiArgs<Args...> {
public:
  MultiArgs(T _t, Args... _args)
    : MultiArgs<Args...>(_args...) {
    t = _t;
  }

public:
  T t;
};