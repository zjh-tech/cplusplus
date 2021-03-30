#pragma once

#include <cinttypes>
#include <string>

using namespace std;

namespace Framework
{
    class NetStream;
}  // namespace Framework

namespace Framework
{
    class IAttachParas
    {
    public:
        virtual void FillNetStream(NetStream& out_stream) = 0;
    };

    class ICoder
    {
    public:
        virtual ~ICoder(){};

        virtual uint32_t GetHeaderLen()         = 0;
        virtual uint32_t GetBodyLen(char* data) = 0;

        virtual bool EncodeBody(string& content) = 0;
        virtual void DecodeBody(string& content) = 0;

        virtual bool ZipBody(string& content)   = 0;
        virtual bool UnzipBody(string& content) = 0;

        virtual void FillNetStream(NetStream& out_stream) = 0;
    };

}  // namespace Framework