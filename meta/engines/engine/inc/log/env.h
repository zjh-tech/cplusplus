/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-12-18 10:32:15
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-12-18 15:14:53
 */

#pragma once

#include "engine/inc/log/logdef.h"
#include "engine/inc/log/logger.h"

using namespace Framework;
using namespace Framework::Log;

#define LOG_INIT(path, filename, consoleflag, level) \
  GLogger = make_shared<Framework::Log::Logger>();   \
  GLogger->Init(path, filename, consoleflag, level);

#define LOG_UNINIT()   \
  if (GLogger) {       \
    GLogger->UnInit(); \
  }

#define LogDebug(fmt, ...) \
  GLogger->AddLogEvent(eLogLevel::DEBUG_LEVEL, false, "{}#{}: " fmt, __FILENAME__, __LINE__, ##__VA_ARGS__)

#define LogInfo(fmt, ...) \
  GLogger->AddLogEvent(eLogLevel::INFO_LEVEL, false, "{}#{}: " fmt, __FILENAME__, __LINE__, ##__VA_ARGS__)

#define LogWarn(fmt, ...) \
  GLogger->AddLogEvent(eLogLevel::WARN_LEVEL, false, "{}#{}: " fmt, __FILENAME__, __LINE__, ##__VA_ARGS__)

#define LogError(fmt, ...) \
  GLogger->AddLogEvent(eLogLevel::ERROR_LEVEL, false, "{}#{}: " fmt, __FILENAME__, __LINE__, ##__VA_ARGS__)

#define LogDebugA(fmt, ...) \
  GLogger->AddLogEvent(eLogLevel::DEBUG_LEVEL, true, "{}#{}: " fmt, __FILENAME__, __LINE__, ##__VA_ARGS__)

#define LogInfoA(fmt, ...) \
  GLogger->AddLogEvent(eLogLevel::INFO_LEVEL, true, "{}#{}: " fmt, __FILENAME__, __LINE__, ##__VA_ARGS__)

#define LogWarnA(fmt, ...) \
  GLogger->AddLogEvent(eLogLevel::WARN_LEVEL, true, "{}#{}: " fmt, __FILENAME__, __LINE__, ##__VA_ARGS__)

#define LogErrorA(fmt, ...) \
  GLogger->AddLogEvent(eLogLevel::ERROR_LEVEL, true, "{}#{}: " fmt, __FILENAME__, __LINE__, ##__VA_ARGS__)
