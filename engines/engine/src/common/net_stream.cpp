/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-03-20 20:30:51
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-01-22 14:00:30
 */

#include "engine/inc/common/net_stream.h"
#include <cstdlib>

#if defined(__linux__)
#  include <arpa/inet.h>
#else
#  include <WinSock2.h>
#endif

using namespace std;

namespace Framework {

uint16_t NetStream::n2hs(uint16_t value) {
  return ntohs(value);
}

uint16_t NetStream::h2ns(uint16_t value) {
  return htons(value);
}

uint32_t NetStream::h2nl(uint32_t val) {
  return htonl(val);
}

uint32_t NetStream::n2hl(uint32_t val) {
  return ntohl(val);
}

uint64_t NetStream::h2nll(uint64_t val) {
  return (((uint64_t)htonl(val)) << 32) + htonl(val >> 32);
}

uint64_t NetStream::n2hll(uint64_t val) {
  return (((uint64_t)ntohl(val)) << 32) + ntohl(val >> 32);
}

NetStream::NetStream() {
  m_buff     = static_cast<char*>(malloc(sDefaultSize));
  m_capacity = sDefaultSize;
}

NetStream::NetStream(const NetStream& r) {
  m_buff     = static_cast<char*>(malloc(r.m_capacity));
  m_capacity = r.m_capacity;
  memcpy(&m_buff[m_wpos], &r.m_buff, m_capacity);
  m_rpos = r.m_rpos;
  m_wpos = r.m_wpos;
}

NetStream::NetStream(const char* buff, size_t size) {
  assert(size != 0);
  m_buff     = static_cast<char*>(malloc(size));
  m_capacity = size;
  memcpy(&m_buff[m_wpos], buff, size);
  m_wpos += size;
}

NetStream::~NetStream() {
  if (m_buff != nullptr) {
    free(m_buff);
    m_buff = nullptr;
  }
}

const char* NetStream::Data() {
  return m_buff;
}

size_t NetStream::Size() {
  return m_wpos;
}

void NetStream::Reset() {
  m_rpos = 0;
  m_wpos = 0;
}

void NetStream::EnsureCapacity(size_t len) {
  if (m_capacity < (m_wpos + len)) {
    m_buff     = static_cast<char*>(realloc(m_buff, m_capacity + len + sDefaultSize));
    m_capacity = m_capacity + len + sDefaultSize;
  }
}

void NetStream::ReadBuff(char* pBuff, size_t len) {
  assert(pBuff);
  assert((m_rpos + len) <= m_wpos);

  memcpy(pBuff, &m_buff[m_rpos], len);
  m_rpos += len;
}

void NetStream::WriteBuff(const char* pBuff, size_t len) {
  EnsureCapacity(len);
  memcpy(&m_buff[m_wpos], pBuff, len);
  m_wpos += len;
}

size_t NetStream::GetRPos() {
  return m_rpos;
}

size_t NetStream::GetWPos() {
  return m_wpos;
}

}  // namespace Framework