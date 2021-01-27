/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2021-01-19 10:56:13
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-01-20 14:19:50
 */

#include "httpproducer.h"
#include "engine/inc/common/utils.hpp"
#include "engine/inc/log/env.h"
#include "httpclient.h"

HttpProducer::HttpProducer()
  : io_context_work(io_context) {
}

void HttpProducer::Push(shared_ptr<HttpClient> ptr) {
  if (!ptr) {
    return;
  }

  asio::post(io_context, [this, ptr]() {
    ptr->OnExecuted();
  });
}

bool HttpProducer::Run(uint32_t count) {
  bool busy = false;

  if (io_context.poll_one() != 0) {
    busy = true;
    for (uint32_t i = 0; i < count; ++i) {
      if (io_context.poll_one() == 0) {
        break;
      }
    }
  }

  return busy;
}