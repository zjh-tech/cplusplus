/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-03-20 11:51:52
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-04-13 15:47:40
 */

#pragma once
#include "engine/inc/common/net/interface.h"
#include "engine/inc/common/singleton.hpp"
#include <functional>
#include <map>
#include <memory>

namespace Framework {
namespace Udp {

  class SNetInterfaceMgr {
  public:
    std::function<std::shared_ptr<ISessionVerify>()> session_verify       = nullptr;
    std::function<std::shared_ptr<IMsgHandler>()>    create_msg_handler   = nullptr;
    std::function<std::shared_ptr<ICodecHandler>()>  create_codec_handler = nullptr;
  };

  class CSession;

  class CSessionCreateFactory : public Singleton<CSessionCreateFactory> {
  public:
    std::shared_ptr<ISessionVerify> GetSessionVerify(uint64_t server_type);

    template <class IMsg, class ICodec, class IVerify = ISessionVerify>
    void AddRelation(uint64_t server_type) {
      SNetInterfaceMgr mgr;
      mgr.session_verify = []() {
        return std::make_shared<IVerify>();
      };

      mgr.create_msg_handler = []() {
        return std::make_shared<IMsg>();
      };

      mgr.create_codec_handler = []() {
        return std::make_shared<ICodec>();
      };
      m_map[server_type] = mgr;
    }

    //UdpClient
    std::shared_ptr<CSession> Create(uint64_t server_type);

    //UdpServer
    std::shared_ptr<CSession> CreateEmptySession();
    void                      FillEmptySession(std::shared_ptr<CSession> session, uint64_t server_type);

  private:
    std::map<uint64_t, SNetInterfaceMgr> m_map;  //server_type - SNetInterfaceMgr
    uint64_t                             m_session_id = 0;
  };

}  // namespace Udp
}  // namespace Framework