
#include "httpclient.h"

#include "engine/inc/common/utils.hpp"
#include "engine/inc/log/env.h"

bool InitCurl()
{
    CURLcode code = ::curl_global_init(CURL_GLOBAL_ALL);
    return (code == CURLE_OK) ? true : false;
}

void UnInitCurl()
{
    ::curl_global_cleanup();
}

HttpClient::HttpClient()
{
}

HttpClient::~HttpClient()
{
    if (curl_ptr)
    {
        ::curl_easy_cleanup(curl_ptr);
        curl_ptr = nullptr;
    }
}

bool HttpClient::Init()
{
    curl_ptr = ::curl_easy_init();
    if (curl_ptr == nullptr)
    {
        LogErrorA("[HttpClient] curl_easy_init Error");
        return false;
    }

    return true;
}

bool HttpClient::DoHttpPost(const char* url, const char* data, size_t data_len, long timeout, long withheader)
{
    reset_option();

    ::curl_easy_setopt(curl_ptr, CURLOPT_HEADER, withheader);
    ::curl_easy_setopt(curl_ptr, CURLOPT_URL, url);
    ::curl_easy_setopt(curl_ptr, CURLOPT_POST, 1);
    ::curl_easy_setopt(curl_ptr, CURLOPT_POSTFIELDS, data);
    ::curl_easy_setopt(curl_ptr, CURLOPT_POSTFIELDSIZE, data_len);
    ::curl_easy_setopt(curl_ptr, CURLOPT_TIMEOUT, timeout);
    ::curl_easy_setopt(curl_ptr, CURLOPT_WRITEFUNCTION, HttpClient::ProcessFunc);
    ::curl_easy_setopt(curl_ptr, CURLOPT_WRITEDATA, this);
    CURLcode code = ::curl_easy_perform(curl_ptr);
    if (code != CURLE_OK)
    {
        LogErrorA("[HttpClient] DoHttpPost Error={}", (uint32_t)code);
    }
    return (code == CURLE_OK) ? true : false;
}

NetStream* HttpClient::GetResponseStream()
{
    return &response_stream;
}

void HttpClient::reset_option()
{
    if (curl_ptr)
    {
        ::curl_easy_reset(curl_ptr);
    }
}

size_t HttpClient::ProcessFunc(void* ptr, size_t size, size_t nmemb, void* usrptr)
{
    HttpClient* http_client_ptr = (HttpClient*)usrptr;
    return http_client_ptr->OnHandler(ptr, size, nmemb);
}

size_t HttpClient::OnHandler(void* data, size_t size, size_t nmemb)
{
    size_t total_size = size * nmemb;
    response_stream.WriteBuff((char*)data, total_size);
    return total_size;
}
