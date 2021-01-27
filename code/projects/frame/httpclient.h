/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2021-01-18 15:20:33
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-01-25 16:03:59
 */

#pragma once

#include "engine/inc/tcp/env.h"
#include <curl/curl.h>

using namespace std;

bool InitCurl();
void UnInitCurl();

class HttpClient {
public:
  HttpClient();
  virtual ~HttpClient();

  bool Init();

  virtual void ExecHttpCmd() {}

  virtual void OnExecuted() {}

public:
  bool DoHttpPost(const char* url, const char* data, size_t data_len, long timeout = 10, long withheader = 0);

  NetStream* GetResponseStream();

private:
  void reset_option();

  size_t OnHandler(void* data, size_t size, size_t nmemb);

  static size_t ProcessFunc(void* ptr, size_t size, size_t nmemb, void* usrptr);

private:
  CURL*     curl_ptr = nullptr;
  NetStream response_stream;
};
