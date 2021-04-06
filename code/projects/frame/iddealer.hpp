#pragma once

#include <cinttypes>

using namespace std;

template <typename T, uint32_t MinID, uint32_t MaxID>
class IDDealer
{
public:
    IDDealer()
    {
        for (uint32_t i = MinID; i < MaxID; ++i)
        {
            dealers[i] = nullptr;
        }
    }

    bool Register(uint32_t msg_id, T t)
    {
        if (msg_id < MinID)
        {
            return false;
        }

        if (msg_id > MaxID)
        {
            return false;
        }

        if (dealers[msg_id] != nullptr)
        {
            return false;
        }

        dealers[msg_id] = t;
        return true;
    }

    void UnRegister(uint32_t msg_id)
    {
        if (MinID <= msg_id && msg_id < MaxID)
        {
            dealers[msg_id] = nullptr;
        }
    }

    T Find(uint32_t msg_id)
    {
        if (MinID <= msg_id && msg_id < MaxID)
        {
            return dealers[msg_id];
        }

        return nullptr;
    }

private:
    T dealers[MaxID - MinID];
};