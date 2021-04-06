#include "centerserver.h"

int main(void)
{
    CenterServer server;

    if (server.Init())
    {
        server.Run();
    }

    server.UnInit();
    return 0;
}