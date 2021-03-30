#pragma once

#include "engine/inc/log/logdefine.h"
#include "engine/inc/log/logger.h"

using namespace Framework;
using namespace Framework::Log;

#define LOG_INIT(path, filename, level)              \
    GLogger = make_shared<Framework::Log::Logger>(); \
    if (GLogger)                                     \
    {                                                \
        GLogger->Init(path, filename, level);        \
    }

#define LOG_UNINIT()       \
    if (GLogger)           \
    {                      \
        GLogger->UnInit(); \
    }

#define LogDebugA(fmt, ...) GLogger->AddLogEvent(eLogLevel::DEBUG_LEVEL, fmt, ##__VA_ARGS__)
// GLogger->AddLogEvent(eLogLevel::DEBUG_LEVEL, "{}#{}: " fmt, __FILENAME__, __LINE__, ##__VA_ARGS__)

#define LogInfoA(fmt, ...) GLogger->AddLogEvent(eLogLevel::INFO_LEVEL, fmt, ##__VA_ARGS__)
// GLogger->AddLogEvent(eLogLevel::INFO_LEVEL, "{}#{}: " fmt, __FILENAME__, __LINE__, ##__VA_ARGS__)

#define LogWarnA(fmt, ...) GLogger->AddLogEvent(eLogLevel::WARN_LEVEL, fmt, ##__VA_ARGS__)
// GLogger->AddLogEvent(eLogLevel::WARN_LEVEL, "{}#{}: " fmt, __FILENAME__, __LINE__, ##__VA_ARGS__)

#define LogErrorA(fmt, ...) GLogger->AddLogEvent(eLogLevel::ERROR_LEVEL, fmt, ##__VA_ARGS__)
// GLogger->AddLogEvent(eLogLevel::ERROR_LEVEL, "{}#{}: " fmt, __FILENAME__, __LINE__, ##__VA_ARGS__)
