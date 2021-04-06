#pragma once
#include "frame/server.h"

class BattleServer : public Server
{
public:
    bool Init() override;
    void Run() override;
};