/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-09 15:45:31
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-04-13 10:12:01
 */

#pragma once

#include "engine/inc/common/singleton.hpp"
#include "engine/inc/udp/ikcp.h"
#include <assert.h>
#include <cinttypes>
#include <map>
#include <memory>
#include <set>

namespace Framework {
namespace Udp {
  class CSession;

  class CKcpMgr : public Singleton<CKcpMgr> {
  public:
    using SessionPtr = std::shared_ptr<CSession>;
    CKcpMgr();
    virtual ~CKcpMgr();

  public:
    void       AddPrepare(uint32_t conv, SessionPtr sesion);
    void       DelPrepare(uint32_t conv);
    SessionPtr GetSessionInPrepare(uint32_t conv);

    void AddUsed(uint32_t conv);
    void AddFree(uint32_t conv);

    uint32_t GetConv();

  private:
    std::map<uint32_t, SessionPtr> m_prepare_map;

    std::set<uint32_t> m_free_set;
    std::set<uint32_t> m_used_set;
    uint32_t           m_conv_start_id = 1;
  };

}  // namespace Udp
}  // namespace Framework