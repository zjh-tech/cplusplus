
#include "asyncmodule.h"

#include <thread>

#include "engine/inc/log/env.h"
#include "staticdata/staticdatamgr.h"

AsyncModule::AsyncModule() : io_context_work(io_context)
{
}

void AsyncModule::StartReloadAllConfig(const char* path)
{
    auto thread_ptr = make_shared<thread>([this, path]() {
        LogInfoA("[Server] ReloadAllConfig Start");
        shared_ptr<StaticDataMgr> mgr = make_shared<StaticDataMgr>();
        auto error_ptr                = mgr->LoadAllCfg(path);
        if (error_ptr != nullptr)
        {
            LogErrorA("[Server] ReloadAllConfig Error=%v", error_ptr->Str());
            return;
        }

        asio::post(io_context, [this, mgr]() {
            GStaticDataMgr = mgr;
            LogInfoA("[Server] ReloadAllConfig End");
        });
    });
}