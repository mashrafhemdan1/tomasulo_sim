#pragma once
#ifndef RSTATIONS
#define RSTATIONS

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "RS.h"
#include "instruction.h"
#include "RegFile.h"
#include "Mem.h"
#include <iostream>
#include <string>
#include <fstream>
using namespace std;
//class instruction { public: string get_type(); bool clk(); void update();  char get_status(); int get_issue(); int get_execute(); int get_write(); bool issue_f(); }; //status is a char
//class Mem {};
//class Reg { public: int get_reg(int index); void set_reg(int index, int value); };
class instruction;
class RS
{
public:
	int N;                //number of reservation stations 
	instruction** Inst; //(1) for first instruction 
	int instCount = 0;        //number of instruction currently in the reservatino station
	bool* busy;
	int* Vj;
	int* Vk;
	RS** Qj;//it's a pointer to the RS using the rs1
	int* Qj_index; //index of the instruction in the RS
	RS** Qk;
	int* Qk_index;
	int* Dest;
	int* A;
	int* result;
	Mem* memory;
	RegFile* Registers;
	vector <RS*>* Rstations;
	string type;

	string get_type();
	bool is_zero();
	RS(int capacity, string new_type, RegFile* RegisterFile, Mem* new_memory, vector <RS*>* new_Rstations);
	bool check_rd(int rs, int seqNo);
	bool add(instruction* new_inst_np, int clk);
	bool processInst(int &target_address, int clk, bool show_details, int* ints_no, int* totalBc, int* missBc);//if true take the target_address
	bool is_empty();
	void RAW_update(int value, RS* reservation_station, int index);
	void flush(int sequence_number, bool show_details);
	bool RSs_empty();
	bool check_inst(instruction* myinst);

};

#endif // !RSTATIONS

