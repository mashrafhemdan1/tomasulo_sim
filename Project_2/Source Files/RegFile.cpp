#include "RegFile.h"

RegFile::RegFile()
{
	Registers[0] = 0;
}

void RegFile::setReg(int index, __int16 value)
{
	if (index != 0 && index <= 7 && index >= 0) Registers[index] = value;
}

__int16 RegFile::getReg(int index)
{
	if (index <= 7 && index >= 0) return Registers[index];
	cout << "Register Index out of range\n";
	return 0;
}

void RegFile::printRegFile()
{
	int width = 6;
	cout << "R0 -- R1 -- R2 -- R3 -- R4 -- R5 -- R6 -- R7\n";
	cout << setw(width) << Registers[0] << setw(width) << Registers[1] << setw(width) << Registers[2] << setw(width) << Registers[3] << setw(width) << Registers[4] << setw(width) <<
		Registers[5] << setw(width) << Registers[6] << setw(width) << Registers[7] << endl;
}

