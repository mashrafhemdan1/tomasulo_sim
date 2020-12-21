#pragma once
#ifndef REGFILE
#define REGFILE

#include <vector>
#include <iomanip>
#include <iostream>
using namespace std;
class RegFile
{
private:
	__int16 Registers[8] = { 0,0,0,0,0,0,0,0 };

public:
	RegFile();
	void setReg(int index, __int16 value);
	__int16 getReg(int index);
	void printRegFile();
};

#endif // !REGFILE

