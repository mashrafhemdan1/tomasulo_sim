#include "instruction.h"
void instruction::clk()
{
	if (stall > 0)
	{
		stall = stall - 1;
	}
	else if (issue > 0)
	{
		issue = issue - 1;
	}
	else if (execute > 0)
	{
		execute = execute - 1;
	}
	else if (write > 0)
	{
		write = write - 1;
	}
	else
	{
		stall = stall;
		issue = issue;
		execute = execute;
		write = write;
	}
}

void instruction::update()
{
	if ((issue == 0) & (execute != 0) & (write != 0))
		status = 'E';
	else if ((issue == 0) & (execute == 0) & (write != 0))
		status = 'W';
	else
		status = status;


}
string instruction::get_type()
{
	return type;
}
int instruction::get_rs1()
{
	return rs1;

}
int instruction::get_rs2()
{
	return rs2;

}
int instruction::get_rd()
{
	return rd;
}
int instruction::get_imm()
{
	return imm;
}
char instruction::get_status()
{
	return status;
}
int instruction::get_issue()
{
	return issue;
}
int instruction::get_execute()
{
	return execute;
}
int instruction::get_write()
{
	return write;
}
int instruction::get_stall()
{
	return stall;
}
void instruction::dec_stall()
{
	stall = stall -1;
}
int instruction::get_seqNo()
{
	return sequence_no;
}
int instruction::get_addr()
{
	return addr;
}

void instruction::set_stall(int stall_new)
{
	stall = stall_new;
}
void instruction::set_rs1(int rs_new)
{
	rs1 = rs_new;
}
void instruction::set_rs2(int rs_new)
{
	rs2 = rs_new;
}
void instruction::set_rd(int rd_new)
{
	rd = rd_new;
}
void instruction::set_imm(int imm_new)
{
	imm = imm_new;
}
void instruction::set_seqNo(int seqNo)
{
	sequence_no = seqNo;
}
void instruction::set_addr(int new_addr)
{
	addr = new_addr;
}
void instruction::set_write(int new_write)
{
	write = new_write;
}
void instruction::set_execute(int new_execute)
{
	execute = new_execute;
	no_execute = execute;
}
void instruction::set_issue(int new_issue)
{
	issue = new_issue;
}
void instruction::set_type(string new_type)
{
	type = new_type;
}
void inst_queue::set_target_addr(int Taddr)
{
	last_mem = Taddr;
}
bool inst_queue::pop(int clk, bool show_details, int* ints_no, int* totalBc, int* missBc)
{ 
	RS* TRstations = nullptr;
	int target_address;
	bool jmp;
	bool ispop = true;
	for (int i = 0; i < reservation_Stations->size(); i++)
	{
		if ((*reservation_Stations)[i]->get_type() == inst[0]->get_type())
		{
			if (((*reservation_Stations)[i]->is_empty()) && inst[0]->end_FT_clk < clk)
			{
					(*reservation_Stations)[i]->add(inst[0], clk);
					if (show_details) cout << "Instruction type (" << inst[0]->get_type() << ") has been added to its corresponding RS\n";
					inst.erase(inst.begin());
			}
			else {
				inst = inst; ispop = false;
			}
			TRstations = (*reservation_Stations)[i];
			break;
		}
		else if (((*reservation_Stations)[i]->get_type() == "ARITHMATIC") && ((inst[0]->get_type() == "ADD") | (inst[0]->get_type() == "ADDI") | (inst[0]->get_type() == "SUB")))
		{
			if (((*reservation_Stations)[i]->is_empty()) && inst[0]->end_FT_clk < clk)
			{
				(*reservation_Stations)[i]->add(inst[0], clk);
				if (show_details) cout << "Instruction type (" << inst[0]->get_type() << ") has been added to its corresponding RS\n";
				inst.erase(inst.begin());
			}
			else {
				inst = inst; ispop = false;
			}
			TRstations = (*reservation_Stations)[i];
			break;
		}
		else if (((*reservation_Stations)[i]->get_type() == "JUMP") && ((inst[0]->get_type() == "JMP") | (inst[0]->get_type() == "JALR") | (inst[0]->get_type() == "RET")))
		{
			if (((*reservation_Stations)[i]->is_empty()) && inst[0]->end_FT_clk < clk)
			{
				(*reservation_Stations)[i]->add(inst[0], clk);
				if (show_details) cout << "Instruction type (" << inst[0]->get_type() << " R" << inst[0]->get_rs1() << " ) has been added to its corresponding RS\n";
				inst.erase(inst.begin());
			}
			else {
				inst = inst; ispop = false;
			}
			TRstations = (*reservation_Stations)[i];
			break;
		}
		else if (inst[0]->get_type() == "NOP")
		{
			inst.erase(inst.begin());
			TRstations = (*reservation_Stations)[i];
			if (show_details) cout << "NOP instruction found. Not added to a RS\n";
		}
		else
		{

			inst = inst;
			TRstations = (*reservation_Stations)[i];

		}
	}
	
	if (ispop && (*reservation_Stations)[0]->RSs_empty()) return true; //why only this reservation station zero why ??
	if (!(*reservation_Stations)[0]->RSs_empty()) {
		jmp = TRstations->processInst(target_address, clk, show_details, ints_no, totalBc, missBc);
		if (jmp)
		{
				set_target_addr(target_address);
				flush = true;
				// put a signal for the queue to flush in the next push
		}

	}
	
	return false;
}
void inst_queue::push(int clk, bool show_details)
{
	bool jmp = false;
	bool stall = false;
	bool pushed = false;
	// note we first flush the queue in case we have a jump taken
	if (flush)
	{
		int seq = inst.size();
		for (int i = 0; i < seq; i++) 
		{
			inst.erase(inst.begin());
		}
		flush = false; // we get the thing to its original signal
	}
	// fetching
	if (inst.size() == 0) {
		for (int i = 0; i < 4; i++) // during the first loadin of the queue load all the entries of the instruction queue
		{
			pushed = false;
			instruction* myinst = make_instruction(INST_MEM[last_mem]);
			myinst->end_FT_clk = clk;
			if (myinst->get_type() == "BEQ" || myinst->get_type() == "JMP" || myinst->get_type() == "JALR" || myinst->get_type() == "RET") myinst->queue_stall = true;
			myinst->set_addr(last_mem);
			last_mem = last_mem + 1; //just as a default
			int queue_size = inst.size();
			for (int j = queue_size - 1; j >= 0; j--) { //if we find a jump, we check dependency and change the last mem if it is resolved
				if (inst[j]->get_type() == "BEQ") {
					if (inst[j]->get_imm() < 0) {
						if (inst[j]->queue_stall) {
							last_mem = inst[j]->get_addr() + inst[j]->get_imm() + 1; jmp = true;
							inst[j]->queue_stall = false;
						}
					}
				}
				else if (inst[j]->get_type() == "JMP") {
					if (inst[j]->queue_stall) {
						last_mem = inst[j]->get_addr() + inst[j]->get_imm() + 1; jmp = true;
						inst[j]->queue_stall = false;
					}
				}
				else if (inst[j]->get_type() == "JALR") {
					if ((*reservation_Stations)[0]->check_rd(inst[j]->get_rs1(), inst[j]->get_seqNo())) {
						stall = true;
					}
					else if (inst[j]->queue_stall) {
						last_mem = my_reg->getReg(inst[j]->get_rs1()); jmp = true;
						inst[j]->queue_stall = false;
					}
				}
				else if (inst[j]->get_type() == "RET") {
					if ((*reservation_Stations)[0]->check_rd(inst[j]->get_rs1(), inst[j]->get_seqNo())) {
						stall = true;
					}
					else if (inst[j]->queue_stall) {
						last_mem = my_reg->getReg(inst[j]->get_rs1()); jmp = true;
						inst[j]->queue_stall = false;
					}
				}
			}
			if (!stall && !pushed) {
				if (jmp) {
					instruction* myinst = make_instruction(INST_MEM[last_mem]);
					myinst->end_FT_clk = clk;
					if (myinst->get_type() == "BEQ" || myinst->get_type() == "JMP" || myinst->get_type() == "JALR" || myinst->get_type() == "RET") myinst->queue_stall = true;
					myinst->set_addr(last_mem);
					if (show_details) cout << INST_MEM[last_mem] << " Fetched " << endl;
					last_mem = last_mem + 1;
				}
				else if (show_details) cout << INST_MEM[last_mem - 1] << " Fetched " << endl;
				inst.push_back(myinst);
			}
		}
	}
	else if (inst.size() < 4)
	{
		pushed = false;
		instruction* myinst = make_instruction(INST_MEM[last_mem]);
		myinst->end_FT_clk = clk;
		if (myinst->get_type() == "BEQ" || myinst->get_type() == "JMP" || myinst->get_type() == "JALR" || myinst->get_type() == "RET") myinst->queue_stall = true;
		myinst->set_addr(last_mem);
		last_mem = last_mem + 1; //just as a default
		int queue_size = inst.size();
		for (int j = queue_size - 1; j >= 0; j--) { //if we find a jump, we check dependency and change the last mem if it is resolved
			if (inst[j]->get_type() == "BEQ") {
				if (inst[j]->get_imm() < 0) {
					if ((*reservation_Stations)[0]->check_rd(inst[j]->get_rs1(), inst[j]->get_seqNo()) || (*reservation_Stations)[0]->check_rd(inst[j]->get_rs2(), inst[j]->get_seqNo()) || check_rd(inst[j]->get_rs2(), j) || check_rd(inst[j]->get_rs1(), j)) { ////-----note17
						stall = true;
					}
					else if(inst[j]->queue_stall){
						last_mem = inst[j]->get_addr() + inst[j]->get_imm() + 1; jmp = true;
						inst[j]->queue_stall = false;
					}
				}
			}
			else if (inst[j]->get_type() == "JMP") {
				if (inst[j]->queue_stall) {
					last_mem = inst[j]->get_addr() + inst[j]->get_imm() + 1; jmp = true;
					inst[j]->queue_stall = false;
				}
			}
			else if (inst[j]->get_type() == "JALR") {
				if ((*reservation_Stations)[0]->check_rd(inst[j]->get_rs1(), inst[j]->get_seqNo()) || check_rd(inst[j]->get_rs1(), j) || check_preJInst(j)) {
					stall = true;
				}
				else if (inst[j]->queue_stall) {
					last_mem = my_reg->getReg(inst[j]->get_rs1()); jmp = true;
					inst[j]->queue_stall = false;
				}
			}
			else if (inst[j]->get_type() == "RET") {
				if ((*reservation_Stations)[0]->check_rd(inst[j]->get_rs1(), inst[j]->get_seqNo()) || check_rd(inst[j]->get_rs1(), j) || check_preJInst(j)) {
					stall = true;
				}
				else if (inst[j]->queue_stall) {
					last_mem = my_reg->getReg(inst[j]->get_rs1()); jmp = true;
					inst[j]->queue_stall = false;
				}
			}
		}
		if (!stall && !pushed) {
			if (jmp) {
				myinst = make_instruction(INST_MEM[last_mem]);
				myinst->end_FT_clk = clk;
				if (myinst->get_type() == "BEQ" || myinst->get_type() == "JMP" || myinst->get_type() == "JALR" || myinst->get_type() == "RET") myinst->queue_stall = true;
				myinst->set_addr(last_mem);
				if (show_details) cout << INST_MEM[last_mem] << " Fetched " << endl;
				last_mem = last_mem + 1;
			}
			else if (show_details) cout << INST_MEM[last_mem-1] << " Fetched " << endl;
			inst.push_back(myinst);
		}
	}
	else {
		last_mem = last_mem;
	}
}
inst_queue::inst_queue(vector<RS*>* new_reservation_Stations, string IM[32], RegFile* regfile)
{
	for (int i = 0; i < 32; i++) {
		INST_MEM[i] = IM[i];
	}
	reservation_Stations = new_reservation_Stations;
	my_reg = regfile;
}
instruction* inst_queue::make_instruction(string text_instruction)
{
	instruction* in = new instruction;
	vector<string> clean;
	stringstream ss(text_instruction);
	string sok = "";

	while (ss.good()) {
		string substr;
		getline(ss, substr, ' ');
		clean.push_back(substr);

	}


	for (int i = 1; i < clean.size(); i++)
	{

		sok = sok + clean[i];
	}


	stringstream sf(sok);

	string substr;
	vector <string> v;

	v.push_back(clean[0]);
	while (sf.good()) {
		string substr;
		getline(sf, substr, ',');
		v.push_back(substr);

	}
	in->set_type(v[0]);
	// instructions with rd in the first f, instructions witout rd in the else
	if ((in->get_type() == "LW") | (in->get_type() == "ADD") | (in->get_type() == "SUB") | (in->get_type() == "ADDI") | (in->get_type() == "MULT") | (in->get_type() == "JALR") | (in->get_type() == "NAND"))
	{
		in->set_rd((int)v[1].at(1) - (int)48);
		if ((in->get_type() == "ADD") | (in->get_type() == "SUB") | (in->get_type() == "MULT") | (in->get_type() == "NAND"))
		{
			in->set_rs1((int)v[2].at(1) - (int)48);
			in->set_rs2((int)v[3].at(1) - (int)48);
		}
		else if ((in->get_type() == "ADDI") | (in->get_type() == "LW"))
		{
			in->set_rs1((int)v[2].at(1) - (int)48);
			in->set_imm(stoi(v[3]));
		}
		else
		{
			in->set_rs1((int)v[2].at(1) - (int)48); // jalr
		}
	}
	else
	{
		if ((in->get_type() == "SW") | (in->get_type() == "BEQ") | (in->get_type() == "RET"))
		{
			in->set_rs1((int)v[1].at(1) - (int)48);
			if ((in->get_type() == "SW") | (in->get_type() == "BEQ"))
			{
				in->set_rs2((int)v[2].at(1) - (int)48);
				in->set_imm(stoi(v[3]));
			}
		}
		else if (in->get_type() == "JMP")
			in->set_imm(stoi(v[1]));
		else
			in->set_type("nop");
	}

	in->set_stall(0);
	in->set_issue(1);
	string ty = in->get_type();
	if (ty == "LW") in->set_execute(2);
	else if (ty == "SW") in->set_execute(2);
	else if (ty == "JMP" || ty == "JALR" || ty == "RET") in->set_execute(1);
	else if (ty == "BEQ") in->set_execute(1);
	else if (ty == "ADD" || ty == "ADDI" || ty == "SUB") in->set_execute(2);
	else if (ty == "NAND") in->set_execute(1);
	else if (ty == "MULT") in->set_execute(8);
	in->set_write(1);
	return in;
}

bool inst_queue::check_rd(int rs, int index)
{
	if (rs == 0) return false;
	for (int j = 0; j < index; j++) {
			if (inst[j]->get_rd() == rs) { //
				return true;//this means there is a RAW dependency
		}
	}
	return false;//no RAW dependency
}

bool inst_queue::check_preJInst(int index)
{
	for (int i = 0; i < index; i++) {
		if (inst[i]->get_type() == "BEQ" || inst[i]->get_type() == "JALR" || inst[i]->get_type() == "JMP" || inst[i]->get_type() == "RET") {
			return true;
		}
	}
	return false;
}

void instruction::print_inst()
{
	cout << type;
	if (type == "ADD" || type == "SUB" || type == "NAND" || type == "MULT") {
		cout << " R" << rd << ", R" << rs1 << ", R" << rs2;
	}
	else if (type == "LW" || type == "ADDI") {
		cout << " R" << rd << ", R" << rs1 << ", " << imm;
	}
	else if (type == "JMP") {
		cout << " " << imm;
	}
	else if (type == "BEQ") {
		cout << " R" << rs1 << ", R" << rs2 << ", " << imm;
	}
	else if (type == "JALR") {
		cout << " R" << rd << ", R" << rs1;
	}
	else if (type == "RET") {
		cout << " R" << rs1;
	}
	else if (type == "SW") {
		cout << " R" << rs1 << ", R" << rs2 << ", " << imm;
	}
}

void instruction::print_clk()
{
	print_inst();
	cout << ": Fetch(" << end_FT_clk << ") / Issue(" << end_IS_clk << ") / Execute(" << end_EX_clk-no_execute+1 << ", " << end_EX_clk << ") / Write(" << end_WB_clk << ")\n";
}

