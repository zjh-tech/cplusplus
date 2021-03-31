/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2021-02-18 11:37:20
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-02-23 16:58:06
 */

#pragma once
#include <hiredis.h>
#include <memory>

using namespace std;

namespace Framework {
namespace MemDB {

  struct ReplyDeleter {
    void operator()(redisReply* reply_ptr) const {
      if (reply_ptr != nullptr) {
        freeReplyObject(reply_ptr);
      }
    }
  };

  using RedisReplyUPtr = unique_ptr<redisReply, ReplyDeleter>;

  namespace Reply {
    inline bool IsError(redisReply& reply) {
      return reply.type == REDIS_REPLY_ERROR;
    }

    inline bool IsNil(redisReply& reply) {
      return reply.type == REDIS_REPLY_NIL;
    }

    inline bool IsString(redisReply& reply) {
      return reply.type == REDIS_REPLY_STRING;
    }

    inline bool IsStatus(redisReply& reply) {
      return reply.type == REDIS_REPLY_STATUS;
    }

    inline bool IsInteger(redisReply& reply) {
      return reply.type == REDIS_REPLY_INTEGER;
    }

    inline bool IsArray(redisReply& reply) {
      return reply.type == REDIS_REPLY_ARRAY;
    }

  }  // namespace Reply
}  // namespace MemDB
}  // namespace Framework