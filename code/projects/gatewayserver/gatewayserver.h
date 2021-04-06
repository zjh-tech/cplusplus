#pragma once
#include "frame/server.h"

class GatewayServer : public Server
{
public:
    bool Init() override;
    void Run() override;
};