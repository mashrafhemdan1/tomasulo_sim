#include "Mem.h"

Mem::Mem()
{
    memory[16] = 20;
}

__int16 Mem::load(int addr)
{
    return memory[addr];
}

void Mem::store(int addr, __int16 value)
{
    memory[addr] = value;
}
