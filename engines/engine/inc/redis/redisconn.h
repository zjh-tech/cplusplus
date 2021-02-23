/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-26 11:51:53
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-02-19 11:45:29
 */

#pragma once

#include "engine/inc/log/env.h"
#include "engine/inc/redis/redisdefine.h"
#include "engine/inc/redis/reply.h"
#include <memory>

using namespace std;

namespace Framework {
namespace MemDB {

  class RedisConn {
  public:
    bool Connect(RedisConnSpec spec);

    template <typename... Args>
    void Send(const char* format, Args&&... args);

    void Send(int argc, const char** argv, const size_t* argvlen);

    RedisReplyUPtr Recv();

  private:
    timeval to_timeval(int64_t msec);

  private:
    shared_ptr<redisContext> redis_ctx_ptr = nullptr;
  };

  template <typename... Args>
  inline void RedisConn::Send(const char* format, Args&&... args) {
    auto ctx = redis_ctx_ptr.get();
    if (ctx == nullptr) {
      LogError("[Redis] Send RedisContext Ptr Is Null");
      return;
    }

    if (redisAppendCommand(ctx, format, forward<Args>(args)...) != REDIS_OK) {
      LogError("[Redis] redisAppendCommand Error");
    }
  }

}  // namespace MemDB
}  // namespace Framework