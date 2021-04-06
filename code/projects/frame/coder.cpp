#include "coder.h"

#include "engine/inc/common/netstream.h"

uint32_t Coder::GetHeaderLen()
{
    return msg_header_size;
}

uint32_t Coder::GetBodyLen(char* data)
{
    uint32_t headerLen = GetHeaderLen();
    NetStream stream(data, headerLen);
    stream >> m_msg_header.encode_flag;
    stream >> m_msg_header.zip_flag;
    stream >> m_msg_header.body_len;
    return m_msg_header.body_len;
}

bool Coder::EncodeBody(string& content)
{
    // string out;
    // content.swap(out);
    return false;
}

void Coder::DecodeBody(string& content)
{
    if (!m_msg_header.encode_flag)
    {
        return;
    }
}

bool Coder::ZipBody(string& content)
{
    return false;
}

bool Coder::UnzipBody(string& content)
{
    if (!m_msg_header.zip_flag)
    {
        return true;
    }

    // ensure unzip max len
    return false;
}

void Coder::FillNetStream(NetStream& out_stream)
{
    string bodyDatas(out_stream.Data(), out_stream.Size());
    bool encode_flag = EncodeBody(bodyDatas);
    bool zip_flag    = ZipBody(bodyDatas);

    MsgHeader header;
    header.encode_flag = encode_flag;
    header.zip_flag    = zip_flag;
    header.body_len    = bodyDatas.size();

    out_stream.Reset();
    out_stream << header.encode_flag;
    out_stream << header.zip_flag;
    out_stream << header.body_len;
    out_stream.WriteBuff(bodyDatas.c_str(), bodyDatas.size());
}
