/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-03-19 18:49:45
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-04-10 17:21:45
 */

#pragma once

#include "engine/inc/common/singleton.hpp"
#include <map>
#include <memory>

namespace Framework {
namespace Udp {

  class CSession;

  class CSessionMgr : public Singleton<CSessionMgr> {
  public:
    using SessionPtr = std::shared_ptr<CSession>;
    virtual ~CSessionMgr();

  public:
    void SendBySessionID(uint32_t conv_id, const char* msg, uint32_t len);

    SessionPtr GetSessionPtr(uint32_t conv_id);

    void AddSession(SessionPtr session);

    void DelSession(uint32_t conv_id);

    void Update();

  private:
    std::map<uint32_t, SessionPtr> m_session_map;  //conv - SessionPtr
  };

}  // namespace Udp
}  // namespace Framework