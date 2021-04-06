#include "dbserver.h"
int main(void) {
  DBServer server;

  if (server.Init()) {
    server.Run();
  }

  server.UnInit();
  return 0;
}