/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-09 15:45:44
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-04-09 16:45:45
 */

#include "engine/inc/udp/kcp_mgr.h"
#include "engine/inc/udp/session.h"

namespace Framework {
namespace Udp {
  CKcpMgr::CKcpMgr() {
  }

  CKcpMgr::~CKcpMgr() {
  }

  void CKcpMgr::AddPrepare(uint32_t conv, SessionPtr sesion) {
    m_prepare_map[conv] = sesion;
  }

  void CKcpMgr::DelPrepare(uint32_t conv) {
    auto iter = m_prepare_map.find(conv);
    if (iter != m_prepare_map.end()) {
      m_prepare_map.erase(iter);
    }
  }

  SessionPtr CKcpMgr::GetSessionInPrepare(uint32_t conv) {
    auto iter = m_prepare_map.find(conv);
    if (iter != m_prepare_map.end()) {
      return iter->second;
    }
    return nullptr;
  }

  void CKcpMgr::AddUsed(uint32_t conv) {
    m_used_set.insert(conv);
  }

  void CKcpMgr::AddFree(uint32_t conv) {
    m_free_set.insert(conv);
  }

  uint32_t CKcpMgr::GetConv() {
    if (m_free_set.size() == 0) {
      uint32_t end_id = m_conv_start_id + 100;
      for (uint32_t i = m_conv_start_id; i < end_id; ++i) {
        m_free_set.insert(i);
      }
    }

    assert(m_free_set.size());
    auto     iter = m_free_set.begin();
    uint32_t conv = *iter;
    m_free_set.erase(iter);
    return conv;
  }

}  // namespace Udp
}  // namespace Framework