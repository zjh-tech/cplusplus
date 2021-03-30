#pragma once
#include <memory>
#include <vector>

#include "engine/inc/tcp/ievent.h"

namespace Framework
{
    namespace Tcp
    {
        class IConnection;

        class Event : public IEvent
        {
        public:
            Event(eEventType evt_type, shared_ptr<IConnection> conn_ptr, shared_ptr<ISession> sess_ptr)
            {
                event_type_ = evt_type;
                conn_ptr_   = conn_ptr;
                sess_ptr    = sess_ptr;
            }

            Event(eEventType evt_type, shared_ptr<IConnection> conn_ptr, shared_ptr<ISession> sess_ptr,const string& datas)
                : datas_(datas)
            {
                event_type_ = evt_type;
                conn_ptr_   = conn_ptr;
                sess_ptr    = sess_ptr;
            }

            virtual ~Event()
            {
                sess_ptr = nullptr;
            }

            eEventType GetType() override
            {
                return event_type_;
            }

            shared_ptr<IConnection> GetConnPtr() override
            {
                return conn_ptr_.lock();
            }

            shared_ptr<ISession> GetSessPtr() override
            {
                return sess_ptr;
            }

            string& GetDatas() override
            {
                return datas_;
            }

        private:
            eEventType event_type_;
            weak_ptr<IConnection> conn_ptr_;
            shared_ptr<ISession> sess_ptr = nullptr;  //临时保存,再放入ISessionFactory中
            string datas_;
        };

    }  // namespace Tcp
}  // namespace Framework
