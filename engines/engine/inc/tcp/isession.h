#pragma once
#include <any>
#include <memory>

using namespace std;

namespace Framework
{
    class ICoder;
    class IAttachParas;
}  // namespace Framework

namespace google
{
    namespace protobuf
    {
        class Message;
    }
}  // namespace google

enum class eSessionType
{
    CONNECT = 1,
    LISTEN  = 2,
};

namespace Framework
{
    class NetStream;
}

using namespace Framework;

namespace Framework
{
    namespace Tcp
    {
        class IConnection;
        class ISessionFactory;

        class ISession
        {
        public:
            virtual ~ISession(){};

            virtual void SetConnectionPtr(shared_ptr<IConnection> connPtr) = 0;
            virtual shared_ptr<IConnection> GetConnectionPtr()             = 0;

            virtual uint64_t GetSessionID()                = 0;
            virtual void SetSessionID(uint64_t session_id) = 0;

            virtual shared_ptr<ICoder> GetCoder()           = 0;
            virtual void SetCoder(shared_ptr<ICoder> coder) = 0;

            virtual shared_ptr<ISessionFactory> GetSessionFactory()              = 0;
            virtual void SetSessionFactory(shared_ptr<ISessionFactory> factotry) = 0;

            virtual void Terminate() = 0;

            virtual void SetListenSessionType()    = 0;
            virtual bool IsListenSessionType()     = 0;
            virtual void SetConnectSessionType()   = 0;
            virtual bool IsConnectSessionType()    = 0;
            virtual void SetAttachData(any attach) = 0;
            virtual any GetAttachData()            = 0;

            virtual void AsyncSendBytes(uint32_t msg_id, const char* msg, uint32_t len,
                                        IAttachParas* attach = nullptr)    = 0;
            virtual void AsyncSendProtoMsg(uint32_t msg_id, const google::protobuf::Message& msg,
                                           IAttachParas* attach = nullptr) = 0;

            virtual void OnEstablish()                              = 0;
            virtual void OnTerminate()                              = 0;
            virtual void ProcessMsg(const char* data, uint32_t len) = 0;
        };

        class ISessionFactory
        {
        public:
            virtual ~ISessionFactory(){};

            //创建和删除必须在同一线程中执行,不然存在锁问题
            virtual shared_ptr<ISession> CreateSession()              = 0;
            virtual void AddSession(shared_ptr<ISession> session_ptr) = 0;
            virtual void RemoveSession(uint64_t session_id)           = 0;
        };
    }  // namespace Tcp

}  // namespace Framework