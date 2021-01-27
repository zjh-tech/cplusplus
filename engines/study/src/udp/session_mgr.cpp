/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-03-19 18:49:53
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-04-13 16:00:05
 */

#include "engine/inc/udp/session_mgr.h"
#include "engine/inc/log/logger.h"
#include "engine/inc/udp/session.h"

namespace Framework {
namespace Udp {

  CSessionMgr::~CSessionMgr() {
    m_session_map.clear();
  }

  void CSessionMgr::AddSession(SessionPtr session) {
    assert(session);
    uint64_t conv_id       = session->get_conv_id();
    m_session_map[conv_id] = session;

    LogInfoA("[Net] AddSession {}", conv_id);
  }

  void CSessionMgr::DelSession(uint32_t conv_id) {
    auto iter = m_session_map.find(conv_id);
    if (iter != m_session_map.end()) {
      m_session_map.erase(iter);
      LogInfoA("[Net] DelSession {}", conv_id);
    }
  }

  SessionPtr CSessionMgr::GetSessionPtr(uint32_t conv_id) {
    auto iter = m_session_map.find(conv_id);
    if (iter != m_session_map.end()) {
      return iter->second;
    }

    return nullptr;
  }

  void CSessionMgr::Update() {
    for (auto iter = m_session_map.begin(); iter != m_session_map.end(); ++iter) {
      assert(iter->second);
      iter->second->kcp_update();
    }
  }

  void CSessionMgr::SendBySessionID(uint32_t conv_id, const char* msg, uint32_t len) {
    auto iter = m_session_map.find(conv_id);
    if (iter != m_session_map.end()) {
      if (iter->second) {
        iter->second->send(msg, len);
      }
    }
  }

}  // namespace Udp
}  // namespace Framework