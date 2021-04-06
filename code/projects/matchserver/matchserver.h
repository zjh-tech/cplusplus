#pragma once
#include "frame/server.h"

class MatchServer : public Server
{
public:
    bool Init() override;
    void Run() override;
};