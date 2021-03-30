/*
 * @Author: zhengjinhong
 * @Date: 2019-11-13 11:26:15
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-04-13 20:28:20
 * @Description: file content
 */

#include <sstream>

#include "engine/inc/log/logger.h"
#include "engine/inc/tcp/env.h"
#ifdef GPERFTOOLS_CPU_MACRO
#include <gperftools/profiler.h>
#endif

#ifdef GPERFTOOLS_MEMORY_MACRO
#include <gperftools/heap-profiler.h>
#endif

#if defined(__linux__)
#include <signal.h>
void sig_handler(int signo)
{
    switch (signo)
    {
        case SIGUSR1:
        {
#ifdef GPERFTOOLS_CPU_MACRO
            static bool cpu_start_flag = false;
            if (!cpu_start_flag)
            {
                cpu_start_flag = true;
                std::stringstream cpu_ss;
                cpu_ss << "cpu.prof";
                ProfilerStart(cpu_ss.str().c_str());
                LogInfoA("recv sig start cpu collection");
            }
            else
            {
                cpu_start_flag = false;
                ProfilerStop();
                LogInfoA("recv sig stop cpu collection");
            }
#endif
        }
        break;
        case SIGUSR2:
        {
#ifdef GPERFTOOLS_MEMORY_MACRO
            static bool memory_start_flag = false;
            if (!memory_start_flag)
            {
                memory_start_flag = true;
                std::stringstream memory_ss;
                memory_ss << "memory";
                HeapProfilerStart(memory_ss.str().c_str());
                LogInfoA("recv sig start memory collection");
            }
            else
            {
                memory_start_flag = false;
                HeapProfilerDump("recv sig");
                HeapProfilerStop();
                LogInfoA("recv sig stop memory collection");
            }
#endif
        }
        break;
    }
}
#endif

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

        // LogInfoA("[Net] Recv STestStructResMsg");

        STestStructReqMsg req;
        req.msg_id = res.msg_id;
        std::memcpy(req.token, res.token, TOkEN_SIZE);
        NetStream req_stream;
        req_stream << req.msg_id;
        req_stream.WriteBuff(req.token, TOkEN_SIZE);
        pSesssion->send(req_stream.Data(), req_stream.Size());
        // LogInfoA("[Net] Send STestStructReqMsg");
    }
};

class CTestEventHandler : public INetEventHandler
{
public:
    void handler_event(ISessionPtr pSesssion, NetEvent event_id) override
    {
        switch (event_id)
        {
            case NetEvent::EVT_CONNECT_CLIENT:
            {
                LogInfoA("[Net] CONNECT_CLIENT sid = {} ", pSesssion->get_session_id());

                for (uint32_t i = 0; i < 300; ++i)
                {
                    STestStructReqMsg req;
                    req.msg_id = 10;
                    NetStream req_stream;
                    req_stream << req.msg_id;
                    req_stream.WriteBuff(req.token, TOkEN_SIZE);
                    pSesssion->send(req_stream.Data(), req_stream.Size());
                }

                // LogInfoA("[Net] Send STestStructReqMsg");
            }
            break;
            case NetEvent::EVT_CONNECT_SERVER:
            {
                LogInfoA("[Net] CONNECT_SERVET sid = {} ", pSesssion->get_session_id());
            }
            break;
            case NetEvent::EVT_DISCONNECT:
            {
                LogInfoA("[Net] DISCONNECT sid = {} ", pSesssion->get_session_id());
            }
            break;

            default:
                break;
        }
    }
};

int main(void)
{
    LOG_INIT("client_example", "debug", true);

    uint32_t thread_num                           = std::thread::hardware_concurrency();
    thread_num                                    = 1;
    std::shared_ptr<ProcessorPool> processor_pool = std::make_shared<ProcessorPool>();
    processor_pool->start(thread_num);
    TCP_INIT();

#if defined(__linux__)
    if (SIG_ERR == signal(SIGUSR1, sig_handler))
    {
        return false;
    }

    if (SIG_ERR == signal(SIGUSR2, sig_handler))
    {
        return false;
    }
#endif

    CSessionCreateFactory::Instance()->AddRelation<CTestEventHandler, CTestMsgHandler, ICodecHandler>(0);

    std::vector<std::shared_ptr<CConnector>> connectorVec;
    for (uint32_t i = 0; i < thread_num; ++i)
    {
        std::shared_ptr<CConnector> connector =
            std::make_shared<CConnector>(processor_pool->get_processor()->get_io_context());
        // connector->start("127.0.0.1", 7000);
        // connector->start("192.168.92.128", 7000);
        connector->start("10.23.119.16", 7000);
        connectorVec.push_back(connector);
    }

    bool busy = false;
    while (true)
    {
        busy = false;
        busy = CMainModule::Instance()->update();

        if (!busy)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    processor_pool->stop();

    TCP_RELEASE();
    LOG_RELEASE();
    return 0;
}
