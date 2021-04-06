#include "matchserver.h"

int main(void)
{
    MatchServer server;

    if (server.Init())
    {
        server.Run();
    }

    server.UnInit();
    return 0;
}