#include "RS.h"
#include "instruction.h"

#include <iostream>
#include <math.h>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <set>
#include <fstream>
using namespace std;

int main()
{

    char mode;
    char con;
    cout << "****************" << "please choose which mode of inserting instruction you wish for" << "****************" << endl;
    cout << "Mode 1: " << endl;
    cout << "enter the instruction manually" << endl;
    cout << "Mode 2: " << endl;
    cout << "choose instructions from a file by their index" << endl;
    cout << "Do you want to choose Mode 1? ( 'y' for mode 1, 'n' for mode 2 )" << endl;
    cin >> mode;
    string finalmem[32];
    int counters = 0;
    bool ri = 1;
    // mode 1
    if (mode == 'y')
    {
        cout << "Enter Instructions:\n";
        while (ri)
        {
            cin.ignore();
            getline(cin, finalmem[counters]);
            counters++;
            cout << "enter another instruction? y/n" << endl;
            cin >> con;
            if (con == 'y')
                ri = 1;
            else
                ri = 0;

        }
        cout << "***********************This is your program***************************" << endl;
        // print your program
        for (int i = 0; i < 32; i++)
        {

            cout << "[" << i << "]" << finalmem[i] << endl;
        }
        cout << "Finished Printing Instrutions\n";
    }
    else
    {
        /*
        string file = "instructions for my ass.txt";

        long fsize = ftell(f);
        char* str = (char*)calloc(fsize + 1, sizeof(char));
        fread(str, 1, fsize, f);
        stringstream ss(str);
        string mem[32];
        string substr;

        int i = 0;
        while (ss.good()) {
            string substr;
            getline(ss, substr, '\0');
            mem[i] = substr;
            i++;

        }

        fclose(f);
        */
        ifstream fin;
        fin.open("Test3_loop.txt");
        if (fin.fail())
        {
            cout << "file opening failed" << endl;
            exit(1);
        }
        int i = 0;
        string mem[32];
        while (!fin.eof())
        {
            getline(fin, mem[i]);
            i++;

        }

        cout << "These are the instructions found in your file, please select by index the instructions you want in the program" << endl;

        for (int i = 0; i < 32; i++)
        {

            cout << "[" << i << "]" << mem[i] << endl;
        }

        cout << "To load all these instructions enter (y). If not, enter (n)\n:";
        string k;
        cin >> k;
        if (k == "y") {
            for (int i = 0; i < 32; i++) {
                finalmem[i] = mem[i];
            }
        }
        else {
            cout << "please choose the index of the instruction you want in your program" << endl;
            while (ri)
            {
                int n;
                cin.ignore();
                cin >> n;
                finalmem[counters] = mem[n];
                counters++;
                cout << "enter another index? y/n" << endl;
                cin >> con;
                if (con == 'y')
                    ri = 1;
                else
                    ri = 0;

            }
        }
        cout << "***********************This is your program***************************" << endl;
        for (int i = 0; i < 32; i++)
        {

            cout << "[" << i << "]" << finalmem[i] << endl;
        }


    }
    bool show_details = false; char accept;
    cout << "If you want a detailed instruction processing, enter (y):"; cin >> accept;
    if (accept == 'y') show_details = true;
    //PROCESSING INSTRUCTIONS
    if (show_details) cout << "Declaring Important Classes...\n";
    RegFile Registers;
    Mem memory;
    vector <RS*> Rstations_vector;
    vector <RS*>* Rstations = &Rstations_vector;
    RS RS_LW(2, "LW", &Registers, &memory, Rstations);
    RS RS_SW(2, "SW", &Registers, &memory, Rstations);
    RS RS_BEQ(2, "BEQ", &Registers, &memory, Rstations);
    RS RS_NAND(1, "NAND", &Registers, &memory, Rstations);
    RS RS_MULT(2, "MULT", &Registers, &memory, Rstations);
    RS RS_ARTH(3, "ARITHMATIC", &Registers, &memory, Rstations);
    RS RS_JMP(2, "JUMP", &Registers, &memory, Rstations);
    Rstations->push_back(&RS_LW);
    Rstations->push_back(&RS_SW);
    Rstations->push_back(&RS_NAND);
    Rstations->push_back(&RS_MULT);
    Rstations->push_back(&RS_ARTH);
    Rstations->push_back(&RS_JMP);
    Rstations->push_back(&RS_BEQ);
    inst_queue InsQue(Rstations, finalmem, &Registers);
    if (show_details) cout << "Finished Creating Registers, Memory, Reservation Stations, and Instruction Queue.\n";
    bool end_of_program = false;
    int cycle_no = 0;
    int* totalBcycles = new int;
    (*totalBcycles) = 0;
    int* missBcycles = new int;
    (*missBcycles) = 0;
    int* no_of_insts = new int;
    (*no_of_insts) = 0;
    while (!end_of_program) {
    //for (int i = 0; i < 12; i++){
        if (show_details) cout << "CLK[" << cycle_no << "] ----------------------------------------------------\n";
        InsQue.push(cycle_no, show_details);
        end_of_program = InsQue.pop(cycle_no, show_details, no_of_insts, totalBcycles, missBcycles);
        InsQue.push(cycle_no, show_details); // to make sure the queue is filled again in case any instruction got issued
        if (show_details) Registers.printRegFile();
        cycle_no++;
    }
    float IPC = (float)(*no_of_insts) / (float)cycle_no;
    cout << "IPC -> " << IPC << endl;
    float BMP = 1;
    if (totalBcycles != 0) BMP = (float)(*missBcycles) / (float)(*totalBcycles);
    cout << "Branch Misprediction percentage -> " << BMP << endl;
    return 0;
}
