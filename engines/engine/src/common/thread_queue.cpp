/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-05-01 15:28:23
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-01-19 11:07:44
 */

#include "engine/inc/common/thread_queue.h"

namespace Framework {

ThreadQueue::ThreadQueue() {
  m_pArray = nullptr;
  m_nHead  = 0;
  m_nTail  = 0;
  m_nSize  = 0;
}

ThreadQueue::~ThreadQueue() {
  if (m_pArray) {
    delete[] m_pArray;
    m_pArray = nullptr;
  }
}

bool ThreadQueue::Init(int nSize) {
  if (nSize <= 0) {
    return false;
  }

  m_pArray = new void*[nSize];
  if (!m_pArray) {
    return false;
  }
  m_nSize = nSize;
  return true;
}

bool ThreadQueue::PushBack(void* ptr) {
  int nDist = m_nTail + m_nSize - m_nHead;
  int nUsed = nDist >= m_nSize ? (nDist - m_nSize) : nDist;
  if (nUsed >= m_nSize - 1)
    return false;

  m_pArray[m_nTail] = ptr;
  if (++m_nTail >= m_nSize)
    m_nTail = 0;

  return true;
}

void* ThreadQueue::PopFront() {
  int nDist = m_nTail + m_nSize - m_nHead;
  int nUsed = nDist >= m_nSize ? (nDist - m_nSize) : nDist;
  if (0 == nUsed) {
    return nullptr;
  }

  void* ptr = m_pArray[m_nHead];
  if (++m_nHead >= m_nSize)
    m_nHead = 0;

  return ptr;
}

}  // namespace Framework
