#include "csclientsession.h"

#include "engine/inc/common/utils.hpp"
#include "engine/inc/log/env.h"
#include "engine/inc/tcp/listener.h"
#include "engine/inc/timer/env.h"
#include "pb/cs_common.pb.h"
#include "pb/cs_msg_id.pb.h"
#include "server.h"

CSClientSession::CSClientSession()
{
    m_timer_register       = make_shared<TimerRegister>();
    m_last_beat_heart_time = Utils::GetMillTime();
}

CSClientSession::~CSClientSession()
{
    if (m_timer_register == nullptr)
    {
        m_timer_register->KillAllTimer();
    }
}

void CSClientSession::OnEstablish()
{
    auto self    = this->shared_from_this();
    auto factory = GetSessionFactory();
    assert(factory);
    factory->AddSession(self);
    m_last_beat_heart_time = Utils::GetMillTime();

    m_handler->OnEstablish(self);

    m_timer_register->AddRepeatTimer(BEAT_HEART_TIME_ID, BEAT_HEART_TIME_DELAY, [this, self]() {
        int64_t now = Utils::GetMillTime();
        if ((m_last_beat_heart_time + BEAT_HEART_MAX_TIME) < now)
        {
            LogErrorA("[CSClientSession] SessionID={} BeatHeart Exception", GetSessionID());
            m_handler->OnBeatHeartError(self);
            Terminate();
        }
    });

    if (IsConnectSessionType())
    {
        m_timer_register->AddRepeatTimer(SEND_BEAT_HEART_TIMER_ID, SEND_BEAT_HEART_TIMER_DELAY, [this, self]() {
            pb::c2s_client_session_ping req;
            AsyncSendProtoMsg(pb::c2s_client_session_ping_id, req);
            LogInfoA("[CSClientSession] SessionID={} Send Ping", GetSessionID());
        });
    }
}

void CSClientSession::OnTerminate()
{
    auto factory = GetSessionFactory();
    assert(factory);
    factory->RemoveSession(GetSessionID());

    m_timer_register->KillAllTimer();

    auto self = this->shared_from_this();
    m_handler->OnTerminate(self);
}

void CSClientSession::OnHandlerMsg(uint32_t msg_id, const char* attach, uint32_t attach_len, const char* msg, uint32_t msg_len)
{
    if (msg_id == (uint32_t)pb::c2s_client_session_ping_id)
    {
        m_last_beat_heart_time = Utils::GetMillTime();
        LogDebugA("[CSClientSession] SessionID={} Recv Ping Send Pong", GetSessionID());
        pb::s2c_client_session_pong pong;
        AsyncSendProtoMsg(pb::s2c_client_session_pong_id, pong);
        return;
    }
    else if (msg_id == (uint32_t)pb::s2c_client_session_pong_id)
    {
        m_last_beat_heart_time = Utils::GetMillTime();
        LogDebugA("[CSClientSession] SessionID={} Recv  Pong", GetSessionID());
        return;
    }

    auto self = this->shared_from_this();
    m_handler->OnHandlerMsg(self, msg_id, msg, msg_len);
    m_last_beat_heart_time = Utils::GetMillTime();
}

shared_ptr<ISession> CSClientSessionMgr::CreateSession()
{
    ++m_session_id;
    shared_ptr<CSClientSession> sessionPtr = make_shared<CSClientSession>();
    assert(sessionPtr);
    sessionPtr->SetSessionID(m_session_id);
    sessionPtr->SetCoder(m_coder);
    sessionPtr->SetHandler(m_handler);
    auto self = this->shared_from_this();
    sessionPtr->SetSessionFactory(self);

    LogInfoA("[CSClientSessionMgr] CreateSession SessionID={}", m_session_id);

    return sessionPtr;
}

void CSClientSessionMgr::AddSession(shared_ptr<ISession> session_ptr)
{
    m_session_map[session_ptr->GetSessionID()] = session_ptr;
}

void CSClientSessionMgr::RemoveSession(uint64_t session_id)
{
    auto iter = m_session_map.find(session_id);
    if (iter != m_session_map.end())
    {
        LogInfoA("[CSClientSessionMgr] RemoveSession SessionID={}", m_session_id);
        m_session_map.erase(iter);
    }
}

shared_ptr<ISession> CSClientSessionMgr::FindSessionBySessionID(uint64_t session_id)
{
    auto iter = m_session_map.find(session_id);
    return (iter != m_session_map.end()) ? iter->second : nullptr;
}

bool CSClientSessionMgr::SendProtoMsgBySessId(uint64_t session_id, uint32_t msg_id, const google::protobuf::Message& msg)
{
    auto isession = FindSessionBySessionID(session_id);
    if (isession == nullptr)
    {
        return false;
    }

    auto session = dynamic_pointer_cast<CSClientSession>(isession);
    assert(session);
    session->AsyncSendProtoMsg(msg_id, msg, nullptr);
    return true;
}

void CSClientSessionMgr::Connect(const string& host, uint32_t port, shared_ptr<ICSClientSessionHandler> handler, shared_ptr<ICoder> coder)
{
    ++m_session_id;
    auto isession = CreateSession();
    assert(isession);
    auto session = dynamic_pointer_cast<CSClientSession>(isession);
    assert(session);

    session->SetSessionID(m_session_id);
    session->SetHandler(handler);
    session->SetCoder(coder);
    session->SetConnectSessionType();

    Net::Instance()->DoConnect(GServer->GetNextIoContext(), host, port, isession);
}

void CSClientSessionMgr::Listen(const string& ip, uint32_t port, shared_ptr<ICSClientSessionHandler> handler, shared_ptr<ICoder> coder, shared_ptr<IOContextPool> io_context_pool)
{
    assert(handler);
    assert(coder);

    setHandler(handler);
    setCoder(coder);

    m_listen  = make_shared<Listener>(io_context_pool);
    auto self = this->shared_from_this();
    m_listen->Start(ip, port, self);
}

shared_ptr<CSClientSessionMgr> GCSClientSessionMgr = make_shared<CSClientSessionMgr>();