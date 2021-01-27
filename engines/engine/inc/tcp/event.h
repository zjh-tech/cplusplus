/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-10-19 16:42:32
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-11-25 15:03:25
 */

#pragma once
#include "engine/inc/tcp/ievent.h"
#include <memory>
#include <vector>

namespace Framework {
namespace Tcp {

  class IConnection;

  class Event : public IEvent {
  public:
    Event(eEventType evtType, shared_ptr<IConnection> connPtr, shared_ptr<ISession> sessPtr) {
      m_event_type = evtType;
      m_conn_ptr   = connPtr;
      m_sess_ptr   = sessPtr;
    }

    Event(eEventType evtType, shared_ptr<IConnection> connPtr, shared_ptr<ISession> sessPtr, const string& datas)
      : m_datas(datas) {
      m_event_type = evtType;
      m_conn_ptr   = connPtr;
      m_sess_ptr   = sessPtr;
    }

    virtual ~Event() {
      m_sess_ptr = nullptr;
    }

    eEventType GetType() override {
      return m_event_type;
    }

    shared_ptr<IConnection> GetConnPtr() override {
      return m_conn_ptr.lock();
    }

    shared_ptr<ISession> GetSessPtr() override {
      return m_sess_ptr;
    }

    string& GetDatas() override {
      return m_datas;
    }

  private:
    eEventType            m_event_type;
    weak_ptr<IConnection> m_conn_ptr;
    shared_ptr<ISession>  m_sess_ptr = nullptr;  //临时保存,再放入ISessionFactory中
    string                m_datas;
  };

}  // namespace Tcp
}  // namespace Framework
