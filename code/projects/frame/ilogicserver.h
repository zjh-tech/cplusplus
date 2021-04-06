#pragma once
#include <cinttypes>
#include <functional>
#include <memory>

using namespace std;

class SSServerSession;

class ILogicServer
{
public:
    virtual ~ILogicServer(){};

    virtual void Init()                                                                                                                                      = 0;
    virtual void OnEstablish(shared_ptr<SSServerSession> sess)                                                                                               = 0;
    virtual void OnTerminate(shared_ptr<SSServerSession> sess)                                                                                               = 0;
    virtual void OnHandlerMsg(shared_ptr<SSServerSession> sess, uint32_t msg_id, const char* attach, uint32_t attach_len, const char* msg, uint32_t msg_len) = 0;

    inline void SetSession(shared_ptr<SSServerSession> sess)
    {
        m_sess_ptr = sess;
    }

    inline shared_ptr<SSServerSession> GetSession()
    {
        return m_sess_ptr.lock();
    }

private:
    weak_ptr<SSServerSession> m_sess_ptr;
};

class ILogicServerFactory
{
public:
    virtual ~ILogicServerFactory(){};

    virtual void SetLogicServer(shared_ptr<SSServerSession> sess) = 0;
};