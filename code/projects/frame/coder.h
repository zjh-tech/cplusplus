/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-03-19 18:16:02
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-02-07 17:48:51
 */

#pragma once
#include "engine/inc/common/icoder.h"
#include "engine/inc/tcp/netdefine.h"
#include <assert.h>
#include <string>

namespace Framework {
class NetStream;
}

using namespace Framework;

#pragma pack(push)
#pragma pack(1)
struct MsgHeader {
  bool     encode_flag = false;
  bool     zip_flag    = false;
  uint32_t body_len    = 0;
};
#pragma pack(pop)

class Coder : public ICoder {
public:
  uint32_t GetHeaderLen() override;
  uint32_t GetBodyLen(char* data) override;

  bool EncodeBody(string& content) override;
  void DecodeBody(string& content) override;

  bool ZipBody(string& content) override;
  bool UnzipBody(string& content) override;

  void FillNetStream(NetStream& out_stream) override;

private:
  MsgHeader m_msg_header;

  const uint32_t msg_header_size = 6;
};
