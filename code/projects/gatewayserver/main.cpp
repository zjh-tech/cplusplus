#include "gatewayserver.h"

int main(void)
{
    GatewayServer server;

    if (server.Init())
    {
        server.Run();
    }

    server.UnInit();
    return 0;
}
