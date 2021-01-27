/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-05-01 15:28:00
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-05-01 15:34:08
 */

#pragma once

namespace Framework {

//unlock queue
class ThreadQueue {
public:
  ThreadQueue();
  virtual ~ThreadQueue();

public:
  bool Init(int nSize);

  bool PushBack(void* ptr);

  void* PopFront();

private:
  void** m_pArray;
  int    m_nHead;
  int    m_nTail;
  int    m_nSize;
};

}  // namespace Framework