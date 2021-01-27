/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2019-12-02 21:40:18
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-11-25 12:12:24
 */

#pragma once

namespace Framework {

class Noncopyable {
public:
  Noncopyable()          = default;
  virtual ~Noncopyable() = default;

private:
  Noncopyable(const Noncopyable&) = delete;
  Noncopyable& operator=(const Noncopyable&) = delete;
};

}  // namespace Framework
