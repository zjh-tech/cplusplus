#pragma once
#include "frame/server.h"

class HallServer : public Server
{
public:
    bool Init() override;
    void Run() override;
};