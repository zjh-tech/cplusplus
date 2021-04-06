#pragma once
#include "engine/inc/tcp/env.h"
#include <cinttypes>
#include <string>

namespace Framework {
class IOContextPool;
}

class IServerFacade {
public:
  virtual ~IServerFacade(){};
  virtual bool Init()   = 0;
  virtual void UnInit() = 0;
  virtual void Run()    = 0;
  virtual void Quit()   = 0;
  virtual bool RegisterSignalHandler();
};

class Server : public IServerFacade {
public:
  virtual ~Server(){};

  inline bool IsQuit() {
    return terminate;
  }

  void Quit();

  inline uint64_t GetServerId() {
    return server_id;
  }

  inline uint32_t GetServerType() {
    return server_type;
  }

  inline const string& GetServerName() {
    return server_name;
  }

  inline const string& GetToken() {
    return token;
  }

  virtual bool Init();

  virtual void UnInit();

  inline asio::io_context& GetNextIoContext() {
    return io_context_pool_ptr->GetNextIoContext();
  }

  inline shared_ptr<IOContextPool> GetIOContextPool() {
    return io_context_pool_ptr;
  }

private:
  bool                      terminate   = false;
  uint64_t                  server_id   = 0;
  uint32_t                  server_type = 0;
  string                    server_name = "";
  string                    token       = "";
  shared_ptr<IOContextPool> io_context_pool_ptr;
};

extern Server* GServer;

#define NET_LOOP_COUNT 100
#define TIMER_LOOP_COUNT 60000
#define DB_LOOP_COUNT 100
#define HTTP_CLIENT_LOOP_COUNT 100
