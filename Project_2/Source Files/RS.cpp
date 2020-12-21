#include "RS.h"
using namespace std;
RS::RS(int capacity, string new_type, RegFile* RegisterFile, Mem* new_memory, vector <RS*>* new_Rstations)
{
	N = capacity;
	Vj = new int[N];
	Vk = new int[N];
	Qj = new RS*[N];
	Qj_index = new int[N];
	Qk = new RS*[N];
	Qk_index = new int[N];
	Dest = new int[N];
	A = new int[N];
	busy = new bool[N];
	Inst = new instruction*[N];
	type = new_type;
	Rstations = new_Rstations;
	Registers = RegisterFile;
	memory = new_memory;
	result = new int[N];
	instCount = 0;        
	for (int i = 0; i < N; i++) {
		busy[i] = false;
	}
}

bool RS::check_rd(int rs, int seqNo)
{
	if (rs == 0) return false;
	for (int j = 0; j < Rstations->size(); j++) {
		for (int i = 0; i < (*Rstations)[j]->N; i++) {
			if ((*Rstations)[j]->busy[i] && (*Rstations)[j]->Inst[i]->get_rd() == rs && (*Rstations)[j]->Inst[i]->get_seqNo() > seqNo) { //
				return true;//this means there is a RAW dependency
			}
		}
	}
	return false;//no RAW dependency
}


bool RS::add(instruction* new_inst_np, int clk)//returns true if it can take this instrution, false if it's fully occupied
{
	if (instCount == N) return false; //we cannot issue this instruction because reservation station is full
	for (int i = 0; i < N; i++) {//loop over the instructions here
	if (!busy[i]) {
	Inst[i] = new instruction;  //Instruction
	{
		Inst[i]->set_addr(new_inst_np->get_addr());
		Inst[i]->set_execute(new_inst_np->get_execute());
		Inst[i]->set_imm(new_inst_np->get_imm());
		Inst[i]->set_issue(new_inst_np->get_issue());
		Inst[i]->set_rd(new_inst_np->get_rd());
		Inst[i]->set_rs1(new_inst_np->get_rs1());
		Inst[i]->set_rs2(new_inst_np->get_rs2());
		//Inst[i]->set_seqNo(new_inst_np->get_seqNo());
		Inst[i]->set_stall(new_inst_np->get_stall());
		Inst[i]->set_type(new_inst_np->get_type());
		Inst[i]->set_write(new_inst_np->get_write());
		Inst[i]->end_FT_clk = new_inst_np->end_FT_clk;
	}
	Inst[i]->set_seqNo(clk);
	//Check Dependency (RAW)
	string type = Inst[i]->get_type();
	Qj[i] = nullptr;
	Qj_index[i] = 0;
	if (type == "ADD" || type == "ADDI" || type == "SUB" || type == "MUL" || type == "NAND" || type == "LW" || type == "SW" || type == "BEQ" || type == "RET" || type == "JALR") Vj[i] = Registers->getReg(Inst[i]->get_rs1());
	else if (type == "JMP") Vj[i] = Inst[i]->get_imm(); //to get the immediate value
	else Vj[i] = 0;
	Qk[i] = nullptr;
	Qk_index[i] = 0;
	if (type == "ADD" || type == "SUB" || type == "MUL" || type == "NAND" || type == "BEQ") Vk[i] = Registers->getReg(Inst[i]->get_rs2());
	else if (type == "LW" || type == "ADDI" || type == "SW") Vk[i] = Inst[i]->get_imm(); //to get the immediate value
	else Vk[i] = 0;
	int CInst_D = 50;
	if (type != "JMP") { //no RAW dependecy for jmp
		for (int j = 0; j < Rstations->size(); j++) {//loop over all reservation stations  to check for dependency
			for (int k = 0; k < (*Rstations)[j]->N; k++) {
				if ((*Rstations)[j]->busy[k]) {
					int instAddr_D = Inst[i]->get_seqNo() - (*Rstations)[j]->Inst[k]->get_seqNo();
					if (!((*Rstations)[j] == this && k == i) && instAddr_D > 0 && instAddr_D <= CInst_D) { //to not compare by itself and ensure (after) dependencies
						//HANDELING BRANCH CONTROL HAZARDS
						if ((*Rstations)[j]->Inst[k]->get_type() == "BEQ") {
							Inst[i]->set_stall(Inst[i]->get_seqNo() - (*Rstations)[j]->Inst[k]->get_seqNo()); ////////////-------------------------------------------CHECK THIS
						} //just to make sure
						//FIRST OPERAND
						if (Inst[i]->get_rs1() != 0 && Inst[i]->get_rs1() == (*Rstations)[j]->Inst[k]->get_rd()) {
							CInst_D = instAddr_D;
							Qj[i] = (*Rstations)[j];
							Qj_index[i] = k;
						}
						//SECOND OPERAND
						if (type == "ADD" || type == "ADDI" || type == "SUB" || type == "MULT" || type == "NAND") { //to ensure it has a second operand
							if (Inst[i]->get_rs2() != 0 && Inst[i]->get_rs2() == (*Rstations)[j]->Inst[k]->get_rd()) {
								CInst_D = instAddr_D;
								Qk[i] = (*Rstations)[j];
								Qk_index[i] = k;
							}
						}
					}
				}
			}

		}
	}
	Dest[i] = Inst[i]->get_rd();  //Destination Register no.
	Inst[i]->end_IS_clk = clk;
	//Inst[i]->clk(); //to issue
	busy[i] = true;
	instCount++;
	return true;
	}
	}
	return false; //this indicates that there is something wrong (done for safty)
}

bool RS::is_empty() {
	if (instCount >= N) return false; //we cannot issue this instruction because reservation station is full
	return true;
}
string RS::get_type()
{
	return type;
}
bool RS::is_zero() {
	if (instCount <= 0) return true;
	return false;
}
void RS::RAW_update(int value, RS* reservation_station, int inst_index)
{
	for (int i = 0; i < Rstations->size(); i++) {
		for (int j = 0; j < (*Rstations)[i]->N; j++) {
			if ((*Rstations)[i]->busy[j]) {
				if ((*Rstations)[i]->Qj[j] == reservation_station && (*Rstations)[i]->Qj_index[j] == inst_index) { //you dectect an instruction that is depending in your value
					//update the V, Q, Q_index
					(*Rstations)[i]->Qj[j] = nullptr;
					(*Rstations)[i]->Qj_index[j] = 0;
					(*Rstations)[i]->Vj[j] = value;
				}
				else if ((*Rstations)[i]->Qk[j] == reservation_station && (*Rstations)[i]->Qk_index[j] == inst_index) {
					//update the V, Q, Q_index
					(*Rstations)[i]->Qk[j] = nullptr;
					(*Rstations)[i]->Qk_index[j] = 0;
					(*Rstations)[i]->Vk[j] = value;
				}
			}
		}
	}
}

void RS::flush(int sequence_number, bool show_details)
{
	for (int i = 0; i < Rstations->size(); i++) {
		for (int j = 0; j < (*Rstations)[i]->N; j++) {
			if ((*Rstations)[i]->busy[j]) {
				if ((*Rstations)[i]->Inst[j]->get_seqNo() > sequence_number) {
					(*Rstations)[i]->Qk[j] = nullptr;
					(*Rstations)[i]->Qk_index[j] = 0;
					(*Rstations)[i]->Vk[j] = 0;
					(*Rstations)[i]->Qj[j] = nullptr;
					(*Rstations)[i]->Qj_index[j] = 0;
					(*Rstations)[i]->Vj[j] = 0;
					(*Rstations)[i]->busy[j] = false;  //this instruction has been flushed
					(*Rstations)[i]->instCount--;
				}
			}
		}
	}
	std::cout << "Processor flushed\n";
}

bool RS::RSs_empty()
{
	for (int i = 0; i < Rstations->size(); i++) {
		for (int j = 0; j < (*Rstations)[i]->N; j++) {
			if ((*Rstations)[i]->busy[j]) return false;
		}
	}
	return true;
}

bool RS::check_inst(instruction* myinst)
{
	for (int i = 0; i < Rstations->size(); i++) {
		for (int j = 0; j < (*Rstations)[i]->N; j++) {
			if ((*Rstations)[i]->Inst[i] == myinst) {
				return true;
			}
		}
	}
	return false;
}


bool RS::processInst(int &target_address, int clk, bool show_details, int* ints_no, int* totalBc, int* missBc) //handeling addition/subtraction/ addition immediate
{
	if (show_details) cout << "Processing Instructions...\n";
	bool jmp = false;
	for (int t = 0; t < Rstations->size(); t++) {
		//cout << "RS " << (*Rstations)[t]->get_type() << " \n";
		if ((*Rstations)[t]->instCount > 0) { //indicating no instructions here to process
			for (int i = 0; i < (*Rstations)[t]->N; i++) { //for each instruction
				if ((*Rstations)[t]->busy[i]) { //begin processing
					if ((*Rstations)[t]->Inst[i]->get_status() == 'W') //to execute instructions first
					{
						if (show_details) { (*Rstations)[t]->Inst[i]->print_inst(); cout << " ... in the Writing Stage\n"; }
						//CHECKING whether we need to stall 
						if ((*Rstations)[t]->Inst[i]->get_stall() != 0) {
							(*Rstations)[t]->Inst[i]->dec_stall();//decrementing stall
						}
						else { //just to make sure again
							//CHECK WAW DEPENDENCY
							string type = (*Rstations)[t]->Inst[i]->get_type();
							if (type != "JMP" && type != "BEQ" && type != "SW" && type != "RET") { //these instruction don't do any writing to registers
								//LOOPING OVER STATIONS
								bool break_break = false;
								for (int j = 0; j < Rstations->size(); j++) {//loop over all reservation stations  to check for dependency
									//LOOPING OVER INSTRUCTIONS
									for (int k = 0; k < (*Rstations)[j]->N; k++) {
										//CHECKING ORDER, BUSY STATIONS
										if (!((*Rstations)[j] == (*Rstations)[t] && k == i) && (*Rstations)[j]->busy[k] && (*Rstations)[t]->Inst[i]->get_seqNo() > (*Rstations)[j]->Inst[k]->get_seqNo()) { //to not compare by itself and ensure (after) dependencies
											//CHECK WAW (DESTINATION REGISTER)
											if ((*Rstations)[t]->Inst[i]->get_rd() == (*Rstations)[j]->Inst[k]->get_rd()) {
												(*Rstations)[t]->Inst[i]->set_stall((*Rstations)[j]->Inst[k]->get_execute() + 1);//this one for writing
											}
											//else if ((*Rstations)[j]->Inst[k]->get_type() == "BEQ") {
											//	Inst[i]->set_stall(Inst[i]->get_addr() - (*Rstations)[j]->Inst[k]->get_addr());
											//} //just to make sure
											break_break = true;
											break;
										}
									}
									if (break_break) break;
								}
							}
							if ((*Rstations)[t]->Inst[i]->get_stall() == 0) { //then write here, because the previous loop don't find a dependency
								if (type != "JMP" && type != "BEQ" && type != "SW" && type != "RET") Registers->setReg((*Rstations)[t]->Inst[i]->get_rd(), (*Rstations)[t]->result[i]);
								if (type == "SW") memory->store((*Rstations)[t]->A[i], (*Rstations)[t]->Vj[i]);   //writing to memory
								(*Rstations)[t]->busy[i] = false;
								(*Rstations)[t]->Qj[i] = nullptr; (*Rstations)[t]->Qj_index[i] = 0; (*Rstations)[t]->Vj[i] = 0;
								(*Rstations)[t]->Qk[i] = nullptr; (*Rstations)[t]->Qk_index[i] = 0; (*Rstations)[t]->Vk[i] = 0;
								RAW_update((*Rstations)[t]->result[i], (*Rstations)[t], i); //this will update Q, Q-index, V values of all RSs we have
								(*Rstations)[t]->instCount--;
								if (show_details) cout << "Clk[" << clk << "] " << (*Rstations)[t]->Inst[i]->get_type() << " Writes its result Successfully\n";
								//values of execute, write, issue should be reset again for future use  ----------------------------------------------------LOOK HERE-----------
								(*Rstations)[t]->Inst[i]->end_WB_clk = clk;
								(*Rstations)[t]->Inst[i]->print_clk();
								(*ints_no)++;
								if (type == "BEQ") (*totalBc)++;
							}
						}
					}
				}
			}
		}
	}
	for (int t = 0; t < Rstations->size(); t++) {
		if ((*Rstations)[t]->instCount > 0) { //indicating no instructions here to process
			//Now for the execution processes (we flipped the order so that an instruction cannot execute and then write in one cycle
			for (int i = 0; i < (*Rstations)[t]->N; i++) { //for each instruction
				if ((*Rstations)[t]->busy[i]) { //begin processing
					if ((*Rstations)[t]->Inst[i]->get_status() == 'E')
					{
						if (show_details) { (*Rstations)[t]->Inst[i]->print_inst(); cout << " Waiting in the Executing Stage\n"; }
						if ((*Rstations)[t]->Inst[i]->get_execute() > 1) //not executed yet --> check dependency
						{
							if ((*Rstations)[t]->Qj[i] != nullptr || (*Rstations)[t]->Qk[i] != nullptr) {
								if ((*Rstations)[t]->Inst[i]->get_stall() != 0) {
									(*Rstations)[t]->Inst[i]->dec_stall();//decrementing stall
								}
							}
							else (*Rstations)[t]->Inst[i]->clk();
						}
						else if ((*Rstations)[t]->Inst[i]->get_execute() == 1) { //if reaches here, this means no dependency
							if ((*Rstations)[t]->Inst[i]->get_type() == "ADD") (*Rstations)[t]->result[i] = (*Rstations)[t]->Vj[i] + (*Rstations)[t]->Vk[i]; //add
							else if ((*Rstations)[t]->Inst[i]->get_type() == "ADDI") (*Rstations)[t]->result[i] = (*Rstations)[t]->Vj[i] + (*Rstations)[t]->Vk[i]; //add immediate
							else if ((*Rstations)[t]->Inst[i]->get_type() == "SUB") (*Rstations)[t]->result[i] = (*Rstations)[t]->Vj[i] - (*Rstations)[t]->Vk[i]; //subtract
							else if ((*Rstations)[t]->Inst[i]->get_type() == "NAND") {
								if ((*Rstations)[t]->Qj[i] != nullptr || (*Rstations)[t]->Qk[i] != nullptr) { //handeling RAW dependecy for NAND only
									if ((*Rstations)[t]->Inst[i]->get_stall() != 0) {
										(*Rstations)[t]->Inst[i]->dec_stall();//decrementing stall
									}
								}
								else {
									(*Rstations)[t]->result[i] = ~((*Rstations)[t]->Vj[i] & (*Rstations)[t]->Vk[i]); //NOR
									(*Rstations)[t]->Inst[i]->clk();
									(*Rstations)[t]->Inst[i]->update(); //update status
									if (show_details) cout << "Clk[" << clk << (*Rstations)[t]->Inst[i]->get_type() << " Executed Successfully\n";
									(*Rstations)[t]->Inst[i]->end_EX_clk = clk;
								}
							}
							else if ((*Rstations)[t]->Inst[i]->get_type() == "MUL") (*Rstations)[t]->result[i] = (*Rstations)[t]->Vj[i] * (*Rstations)[t]->Vk[i]; //multipy
							else if ((*Rstations)[t]->Inst[i]->get_type() == "RET") (*Rstations)[t]->A[i] = (*Rstations)[t]->Vj[i]; //subtract
							else if ((*Rstations)[t]->Inst[i]->get_type() == "BEQ") {
								(*Rstations)[t]->A[i] = (*Rstations)[t]->Inst[i]->get_addr() + 1 + (*Rstations)[t]->Inst[i]->get_imm();
								if ((*Rstations)[t]->Vj[i] != (*Rstations)[t]->Vk[i] && (*Rstations)[t]->Inst[i]->get_imm() < 0) {// if both operands are not equal, you take next instruction
									(*Rstations)[t]->flush((*Rstations)[t]->Inst[i]->get_addr() + 1, show_details);
									target_address = (*Rstations)[t]->Inst[i]->get_addr() + 1;
									jmp = true;
									if (show_details) cout << "Clk[" << clk << (*Rstations)[t]->Inst[i]->get_type() << " Executed Successfully\n";
									(*Rstations)[t]->Inst[i]->clk();
									(*Rstations)[t]->Inst[i]->update(); //update status
									(*Rstations)[t]->Inst[i]->end_EX_clk = clk;
									(*missBc)++;
									break;
								}
								else if ((*Rstations)[t]->Vj[i] == (*Rstations)[t]->Vk[i] && (*Rstations)[t]->Inst[i]->get_imm() > 0) {
									(*Rstations)[t]->flush((*Rstations)[t]->Inst[i]->get_seqNo(), show_details);
									target_address = (*Rstations)[t]->A[i];
									jmp = true;
									if (show_details) cout << "Clk[" << clk << (*Rstations)[t]->Inst[i]->get_type() << " Executed Successfully\n";
									(*Rstations)[t]->Inst[i]->clk();
									(*Rstations)[t]->Inst[i]->update(); //update status
									(*Rstations)[t]->Inst[i]->end_EX_clk = clk;
									(*missBc)++;
									break;
								}
							}//subtract
							else if ((*Rstations)[t]->Inst[i]->get_type() == "JMP") (*Rstations)[t]->A[i] = (*Rstations)[t]->Inst[i]->get_imm() + 1; //subtract
							else if ((*Rstations)[t]->Inst[i]->get_type() == "JALR") { (*Rstations)[t]->result[i] = (*Rstations)[t]->Inst[i]->get_addr() + 1; }//subtract
							else if ((*Rstations)[t]->Inst[i]->get_type() == "SW") (*Rstations)[t]->A[i] = (*Rstations)[t]->Vk[i] + (*Rstations)[t]->Inst[i]->get_imm(); //computing address
							else if ((*Rstations)[t]->Inst[i]->get_type() == "LW") {
								(*Rstations)[t]->A[i] = (*Rstations)[t]->Vj[i] + (*Rstations)[t]->Inst[i]->get_imm(); (*Rstations)[t]->result[i] = memory->load((*Rstations)[t]->A[i]);
							}  //subtract
							if (!((*Rstations)[t]->Inst[i]->get_type() == "NAND")) {
								(*Rstations)[t]->Inst[i]->clk();
								(*Rstations)[t]->Inst[i]->update(); //update status
								if (show_details) cout << "Clk[" << clk << (*Rstations)[t]->Inst[i]->get_type() << " Executed Successfully\n";
								(*Rstations)[t]->Inst[i]->end_EX_clk = clk;
							}
						}
					}
				}
			}
		}
	}

	for (int t = 0; t < Rstations->size(); t++) {
		if ((*Rstations)[t]->instCount > 0) { //indicating no instructions here to process
			//now with the issue stage
			for (int i = 0; i < (*Rstations)[t]->N; i++) { //for each instruction
				if ((*Rstations)[t]->busy[i]) { //begin processing
					if ((*Rstations)[t]->Inst[i]->get_status() == 'I')
					{
						if (show_details) { (*Rstations)[t]->Inst[i]->print_inst(); cout << " ... in the Issuing Stage\n"; }
						if ((*Rstations)[t]->Inst[i]->get_issue() == 1) //not executed yet --> check dependency
						{
							(*Rstations)[t]->Inst[i]->clk();
							//cout << "Clk[" << clk << "] " << (*Rstations)[t]->Inst[i]->get_type() << " Issued Successfully\n";
							(*Rstations)[t]->Inst[i]->update();
						}
						else {
							if (show_details) cout << "Something Wrong happened in the Issue Stage:" << "Inst[";
							if (show_details) (*Rstations)[t]->Inst[i]->print_inst();
							if (show_details) cout << "] I_cycles(" << (*Rstations)[t]->Inst[i]->get_issue() << ")\n";  return false; //there is something wrong happened
						}
					}
				}
			}
		}
	}
	return jmp;
}

