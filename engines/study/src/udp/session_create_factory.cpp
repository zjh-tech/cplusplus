/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-03-20 11:52:28
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-04-13 15:49:21
 */

#include "engine/inc/udp/session_create_factory.h"
#include "engine/inc/udp/session.h"

namespace Framework {
namespace Udp {

  std::shared_ptr<ISessionVerify> CSessionCreateFactory::GetSessionVerify(uint64_t server_type) {
    std::shared_ptr<ISessionVerify> session_verify = nullptr;

    auto iter = m_map.find(server_type);
    if (iter != m_map.end()) {
      session_verify = iter->second.session_verify();
    }

    if (!session_verify) {
      session_verify = std::make_shared<ISessionVerify>();
    }

    return session_verify;
  }

  std::shared_ptr<CSession> CSessionCreateFactory::CreateEmptySession() {
    ++m_session_id;
    std::shared_ptr<CSession> session = std::make_shared<CSession>(m_session_id);
    return session;
  }

  void CSessionCreateFactory::FillEmptySession(std::shared_ptr<CSession> session, uint64_t server_type) {
    std::shared_ptr<ICodecHandler> codec = nullptr;
    std::shared_ptr<IMsgHandler>   msg   = nullptr;

    auto iter = m_map.find(server_type);
    if (iter != m_map.end()) {
      codec = iter->second.create_codec_handler();
      msg   = iter->second.create_msg_handler();
    }

    //not fine use default interface
    if (!codec) {
      codec = std::make_shared<ICodecHandler>();
    }

    if (!msg) {
      msg = std::make_shared<IMsgHandler>();
    }

    if (session) {
      session->set_codec_handler(codec);
      session->set_msg_handler(msg);
    }
  }

  std::shared_ptr<CSession> CSessionCreateFactory::Create(uint64_t server_type) {
    ++m_session_id;
    std::shared_ptr<CSession> session = std::make_shared<CSession>(server_type, m_session_id);
    assert(session);
    FillEmptySession(session, server_type);
    return session;
  }

}  // namespace Udp
}  // namespace Framework