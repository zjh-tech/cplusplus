/*
 * @Descripttion:
 * @Author: zhengjinhong
 * @Date: 2020-03-31 17:51:45
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-04-13 20:28:41
 */

#include "engine/inc/log/logger.h"
#include "engine/inc/udp/env.h"

using namespace Framework::Udp;

#define TOkEN_SIZE 32

struct STestStructReqMsg
{
    uint32_t msg_id        = 0;
    char token[TOkEN_SIZE] = {0};
};

struct STestStructResMsg
{
    uint32_t msg_id        = 0;
    char token[TOkEN_SIZE] = {0};
};

class CTestMsgHandler : public IMsgHandler
{
public:
    void handler_msg(ISessionPtr pSesssion, const char* data, uint32_t len) override
    {
        NetStream res_stream(data, len);
        STestStructResMsg res;
        res_stream >> res.msg_id;
        res_stream.ReadBuff(res.token, TOkEN_SIZE);

        LogInfoA("[Net] Recv STestStructResMsg");

        STestStructReqMsg req;
        req.msg_id = res.msg_id;
        std::memcpy(req.token, res.token, TOkEN_SIZE);
        NetStream req_stream;
        req_stream << req.msg_id;
        req_stream.WriteBuff(req.token, TOkEN_SIZE);
        pSesssion->send(req_stream.Data(), req_stream.Size());
        LogInfoA("[Net] Send STestStructReqMsg");
    }
};

int main(void)
{
    LOG_INIT("udp_client_example", "debug", true);

    uint32_t thread_num                           = std::thread::hardware_concurrency();
    thread_num                                    = 1;
    std::shared_ptr<ProcessorPool> processor_pool = std::make_shared<ProcessorPool>();
    processor_pool->start(thread_num);
    UDP_INIT();

    CSessionCreateFactory::Instance()->AddRelation<CTestMsgHandler, ICodecHandler>(0);

    std::shared_ptr<CUdpClient> client =
        std::make_shared<CUdpClient>(processor_pool->get_processor()->get_io_context());
    const char* local_ip  = "127.0.0.1";
    uint16_t local_port   = 7002;
    const char* remote_ip = "127.0.0.1";
    uint16_t remote_port  = 7001;
    uint64_t server_type  = 0;
    uint32_t conv         = 1;  // tcp send client

    client->Start(local_ip, local_port, remote_ip, remote_port, server_type, conv);
    uint32_t loop_num = 1;
    for (uint32_t i = 0; i < loop_num; ++i)
    {
        STestStructReqMsg req;
        req.msg_id = 10;
        NetStream req_stream;
        req_stream << req.msg_id;
        req_stream.WriteBuff(req.token, TOkEN_SIZE);
        client->GetSessionPtr()->send(req_stream.Data(), req_stream.Size());
        LogInfoA("[Net] Send STestStructReqMsg");
    }

    bool busy = false;
    while (true)
    {
        busy = false;

        if (CMainModule::Instance()->update())
        {
            busy = true;
        }
        if (client->Update())
        {
            busy = true;
        }

        if (!busy)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    processor_pool->stop();
    UDP_RELEASE();
    LOG_RELEASE();

    return 0;
}