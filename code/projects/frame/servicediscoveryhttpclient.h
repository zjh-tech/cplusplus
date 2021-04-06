
#pragma once

#include <memory>

#include "common/msgidrange.h"
#include "engine/inc/timer/env.h"
#include "frame/iddealer.hpp"
#include "httpclient.h"

using namespace std;

typedef void (*SDClientHttpCbFunc)();

class ServiceDiscoveryHttpClient : public enable_shared_from_this<ServiceDiscoveryHttpClient>
{
public:
    ServiceDiscoveryHttpClient();

    bool Init(const string& _url, uint64_t _server_id, const string& _token, SDClientHttpCbFunc _cb_func);

    inline bool GetInitFlag()
    {
        return init_flag;
    }

    inline void SetInitFlag()
    {
        init_flag = true;
    }

    inline SDClientHttpCbFunc GetCbFunc()
    {
        return cb_func;
    }

private:
    bool init_flag                            = false;
    SDClientHttpCbFunc cb_func                = nullptr;
    shared_ptr<ITimerRegister> timer_register = nullptr;
    string all_url                            = "";
    uint64_t server_id                        = 0;
    string token                              = "";

    enum eTimerID : uint32_t
    {
        SERVICEDISCOVERY_HTTP_TIMER_ID = 1,
    };

    enum eTimerDelay : uint64_t
    {
        SERVICEDISCOVERY_HTTP_TIMER_DELAY = 1000 * 3,
    };
};

class SDHttpClient : public HttpClient
{
public:
    SDHttpClient(string _all_url, uint64_t _server_id, string _token);

    virtual ~SDHttpClient();

    void ExecHttpCmd() override;

    void OnExecuted() override;

private:
    string all_url     = "";
    uint64_t server_id = 0;
    string token       = "";
};

typedef bool (*SDHttpFunc)(const char* msg, uint32_t msg_len);

class SDHttpServerSession
{
public:
    bool Init();

    void OnHandlerMsg(uint32_t msg_id, const char* data, uint32_t len);

private:
    IDDealer<SDHttpFunc, SD_MIN_ID, SD_MAX_ID> iddealer;
};

bool OnHandlerHttpServiceDiscoveryAck(const char* msg, uint32_t msg_len);

extern shared_ptr<ServiceDiscoveryHttpClient> GServiceDiscoveryHttpClient;
extern shared_ptr<SDHttpServerSession> GSDHttpServerSession;

#define SD_HTTP_INIT()                                                       \
    HttpConsumer::Instance()->Init();                                        \
    GServiceDiscoveryHttpClient = make_shared<ServiceDiscoveryHttpClient>(); \
    GSDHttpServerSession        = make_shared<SDHttpServerSession>();        \
    GSDHttpServerSession->Init();
