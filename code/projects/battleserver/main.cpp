#include "battleserver.h"

int main(void)
{
    BattleServer server;

    if (server.Init())
    {
        server.Run();
    }

    server.UnInit();
    return 0;
}