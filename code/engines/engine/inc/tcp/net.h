/*
 * @Author: zhengjinhong
 * @Date: 2019-11-12 16:32:48
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-12-03 14:15:09
 * @Description: file content
 */

#pragma once

#include "engine/inc/common/singleton.hpp"
#include "engine/inc/tcp/inet.h"
#include <map>

namespace Framework {
namespace Tcp {

  class Net : public Singleton<Net>, public INet {
  public:
    Net();

    asio::io_context& GetIoContext() {
      return m_io_context;
    }

    void PushEvent(shared_ptr<IEvent> event) override;

    //host 可以是域名,也可以是IP
    void DoConnect(asio::io_context& io_context, const string& host, uint32_t port, shared_ptr<ISession> session_ptr) override;

    bool Run(uint32_t count) override;

    void Stop();

  private:
    asio::io_context       m_io_context;
    asio::io_context::work m_io_worker;
  };

}  // namespace Tcp
}  // namespace Framework