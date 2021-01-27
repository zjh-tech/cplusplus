/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-10-19 16:01:45
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-11-25 15:03:43
 */

#pragma once
#include <any>
#include <memory>
#include <vector>

using namespace std;
namespace Framework {
namespace Tcp {

  class IConnection;

  enum class eEventType {
    ConnEstablish = 1,
    RecvMsg       = 2,
    ConnTerminate = 3,
  };

  class IEvent {
  public:
    virtual ~IEvent(){};
    virtual eEventType              GetType()    = 0;
    virtual shared_ptr<IConnection> GetConnPtr() = 0;
    virtual shared_ptr<ISession>    GetSessPtr() = 0;
    virtual string&                 GetDatas()   = 0;
  };
}  // namespace Tcp
}  // namespace Framework