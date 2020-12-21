#pragma once

#ifndef Instruction
#define Instruction

#include <iostream>
#include <math.h>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <set>
#include "RS.h"
using namespace std;
class RS;
class RegFile;
class instruction
{

private:
	string type = "";
	int rs1 = 0;
	int rs2 = 0;
	int rd = 0;
	int imm = 0;
	char status = 'I';
	int issue = 0;
	int execute = 0;
	int write = 0;
	int stall = 0;
	int addr = 0;
	int sequence_no = 0;


public:
	void clk();
	void update();
	string get_type();
	int get_rs1();
	int get_rs2();
	int get_rd();
	int get_imm();
	char get_status();
	int get_issue();
	int get_execute();
	int get_write();
	int get_stall();
	void dec_stall();
	int get_seqNo();
	int get_addr();
	void set_type(string new_type);
	void set_stall(int stall_new);
	void set_rs1(int rs_new);
	void set_rs2(int rs_new);
	void set_rd(int rd_new);
	void set_imm(int imm_new);
	void set_seqNo(int seqNo);
	void set_addr(int new_addr);
	void set_write(int new_write);
	void set_execute(int new_execute);
	void set_issue(int new_issue);
	int end_IS_clk = 0;
	int start_EX_clk = 0;
	int end_EX_clk = 0;
	int end_WB_clk = 0;
	int end_FT_clk = 0;
	bool queue_stall = false;
	int no_execute = 0;
	void print_inst();
	void print_clk();
};
class inst_queue
{

private:

	vector <instruction*> inst;
	vector <RS*>* reservation_Stations;
	RegFile* my_reg;
	string INST_MEM[32];
	int last_mem = 0;
	bool flush = 0;

public:
	void set_target_addr(int Taddr);
	bool pop(int clk, bool show_details, int* ints_no, int* totalBc, int* missBc);
	void push(int clk, bool show_details);
	inst_queue(vector <RS*>* new_reservation_Stations, string IM[32], RegFile* regfile);
	instruction* make_instruction(string text_instruction);
	bool check_rd(int rs, int index);
	bool check_preJInst(int index);

};
#endif // !Instruction


