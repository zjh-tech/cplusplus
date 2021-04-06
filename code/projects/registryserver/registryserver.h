
#pragma once
#include "frame/server.h"

class RegistryServer : public Server
{
public:
    bool Init() override;
    void Run() override;
};