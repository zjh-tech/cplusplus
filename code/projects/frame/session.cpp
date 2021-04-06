#include "session.h"
#include "engine/inc/log/env.h"
#include "google/protobuf/message.h"
#include <functional>
#include <map>
#include <memory>

using namespace Framework;
using namespace Framework::Tcp;

void CSession::Terminate() {
  auto connPtr = GetConnectionPtr();
  if (connPtr == nullptr) {
    LogWarnA("[CSession] SessionID={} Terminate ConnPtr Is Null", m_session_id);
    return;
  }

  LogInfoA("[CSession] SessionID={} Terminate", m_session_id);

  connPtr->Terminate();
}

void CSession::ProcessMsg(const char* data, uint32_t len) {
  uint32_t        msg_id     = 0;
  static uint32_t msg_id_len = sizeof(uint32_t);

  if (len < msg_id_len) {
    LogErrorA("[CSession] SessionID={},MsgID Len Not Enough", GetSessionID());
    return;
  }

  NetStream in(data, len);
  in >> msg_id;
  uint16_t        attach_len     = 0;
  static uint32_t attach_len_len = sizeof(uint16_t);
  in >> attach_len;

  uint32_t msg_start_index = msg_id_len + attach_len_len + attach_len;

  if (attach_len != 0) {
    OnHandlerMsg(msg_id, data + msg_id_len + attach_len_len, attach_len, data + msg_start_index, len - msg_start_index);
  } else {
    OnHandlerMsg(msg_id, nullptr, 0, data + msg_start_index, len - msg_start_index);
  }
}

void CSession::AsyncSendBytes(uint32_t msg_id, const char* msg, uint32_t len, IAttachParas* attach) {
  auto connPtr = GetConnectionPtr();
  if (connPtr == nullptr) {
    LogWarnA("[CSession] SessionID={} AsyncSendBytes ConnPtr Is Null", m_session_id);
    return;
  }

  if (m_coder == nullptr) {
    LogWarnA("[CSession] SessionID={} AsyncSendBytes CoderPtr Is Null", m_session_id);
    return;
  }

  NetStream out;
  out << msg_id;
  if (attach != nullptr) {
    NetStream attach_stream;
    attach->FillNetStream(attach_stream);
    uint16_t attach_len = attach_stream.Size();
    out << attach_len;
    out.WriteBuff(attach_stream.Data(), attach_stream.Size());
  } else {
    uint16_t attach_len = 0;
    out << attach_len;
  }
  out.WriteBuff(msg, len);
  m_coder->FillNetStream(out);
  connPtr->AsyncSend(out.Data(), out.Size());
}

void CSession::AsyncSendProtoMsg(uint32_t msg_id, const google::protobuf::Message& msg, IAttachParas* attach) {
  char buffer[MAX_MESSAGE_SIZE] = {0};
  msg.SerializeToArray(buffer, MAX_MESSAGE_SIZE);
  uint32_t len = msg.GetCachedSize();
  AsyncSendBytes(msg_id, buffer, len, attach);
}