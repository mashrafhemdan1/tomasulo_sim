#pragma once
#ifndef MEM
#define MEM

#include <string>
class Mem
{
private:
	__int16 memory[512]; //it should be 65536 corresponding to 128 KB but the program recommends decreasing this a little bit as we don't need it. We decrease it to 1KB

public:
	Mem();
	__int16 load(int addr);
	void store(int addr, __int16 value);
};

#endif // !MEM
