/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2019-12-02 21:40:18
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-11-25 12:12:38
 */
#pragma once

#include "engine/inc/common/noncopyable.hpp"
#include <mutex>

namespace Framework {

template <typename T>
class Singleton : Noncopyable {
public:
  static T* Instance() {
    if (m_ptr) {
      return m_ptr;
    }

    std::call_once(m_once_flag, [&]() { m_ptr = new T(); });
    return m_ptr;
  }

  static void Destroy() {
    if (m_ptr) {
      delete m_ptr;
      m_ptr = nullptr;
    }
  }

private:
  static std::once_flag m_once_flag;
  static T*             m_ptr;
};

template <typename T>
std::once_flag Singleton<T>::m_once_flag;

template <typename T>
T* Singleton<T>::m_ptr = nullptr;

}  // namespace Framework
