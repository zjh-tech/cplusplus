#include "hallserver.h"

int main(void)
{
    HallServer server;

    if (server.Init())
    {
        server.Run();
    }

    server.UnInit();
    return 0;
}