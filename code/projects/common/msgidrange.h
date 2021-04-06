#pragma once

#include <cinttypes>

//TS 微服务
//G ==> Gateway
//R ==> Rank
enum eMsgIDRange : uint32_t {
  TS_GR_MIN_ID = 0,
  TS_GR_MAX_ID = 1000,

  SD_MIN_ID = 0,
  SD_MAX_ID = 50,

  GATEWAY_CLIENT_MIN_ID = 0,
  GATEWAY_CLIENT_MAX_ID = 1000,
};