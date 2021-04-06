
#pragma once
#include "frame/server.h"

class DBServer : public Server
{
public:
    bool Init() override;
    void Run() override;
    void UnInit() override;
};