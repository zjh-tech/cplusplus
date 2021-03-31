/*
 * @Descripttion:
 * @Author: zhengjinhong
 * @Date: 2020-04-26 11:52:02
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-02-19 11:55:29
 */

#include "engine/inc/redis/redisconn.h"

#include <chrono>

#ifdef _MSC_VER
#include <winsock2.h>
#endif

namespace Framework
{
    namespace MemDB
    {
        bool RedisConn::Connect(RedisConnSpec spec)
        {
            redisContext* ctx = redisConnectWithTimeout(spec.Host.c_str(), spec.Port, to_timeval(spec.ConnectTimeOut));
            if (ctx == nullptr)
            {
                return false;
            }

            if (ctx->err != REDIS_OK)
            {
                return false;
            }

            if (redisSetTimeout(ctx, to_timeval(spec.CmdTimeOut)) != REDIS_OK)
            {
                LogErrorA("[Redis] redisSetTimeout Error");
                return false;
            }

            if (redisEnableKeepAlive(ctx) != REDIS_OK)
            {
                LogErrorA("[Redis] redisEnableKeepAlive Error");
                return false;
            }

            redis_ctx_ptr = shared_ptr<redisContext>(ctx, [](redisContext* ctx_ptr) {
                // RedisContextDeleter
                if (ctx_ptr != nullptr)
                {
                    redisFree(ctx_ptr);
                    ctx_ptr = nullptr;
                }
            });

            return true;
        }

        void RedisConn::Send(int argc, const char** argv, const size_t* argvlen)
        {
            auto ctx = redis_ctx_ptr.get();
            if (ctx == nullptr)
            {
                LogErrorA("[Redis] Send RedisContext Ptr Is Null");
                return;
            }
            if (redisAppendCommandArgv(ctx, argc, argv, argvlen) != REDIS_OK)
            {
                LogErrorA("[Redis] redisAppendCommandArgv Error");
            }
        }

        RedisReplyUPtr RedisConn::Recv()
        {
            auto ctx = redis_ctx_ptr.get();
            void* r  = nullptr;
            if (redisGetReply(ctx, &r) != REDIS_OK)
            {
                LogErrorA("[Redis] Recv redisGetReply Error");
                return nullptr;
            }

            redisReply* reply = static_cast<redisReply*>(r);
            auto reply_uptr   = RedisReplyUPtr(reply);
            if (Reply::IsError(*reply))
            {
                LogErrorA("[Redis] Recv redisGetReply Error");
                return nullptr;
            }
            return reply_uptr;
        }

        timeval RedisConn::to_timeval(int64_t mill_sec)
        {
            chrono::milliseconds time_out_sec(mill_sec);
            auto sec  = chrono::duration_cast<chrono::seconds>(time_out_sec);
            auto msec = chrono::duration_cast<chrono::microseconds>(time_out_sec - sec);
            timeval tv;
            tv.tv_sec  = sec.count();
            tv.tv_usec = msec.count();
            return tv;
        }

    }  // namespace MemDB
}  // namespace Framework
