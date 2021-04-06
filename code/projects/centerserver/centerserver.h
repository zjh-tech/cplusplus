#pragma once
#include "frame/server.h"

class CenterServer : public Server
{
public:
    bool Init() override;
    void Run() override;
};