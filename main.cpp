#include <iostream>
#include "TomasuloSimulator.h"
using namespace std;


int main() {
    vector<Instruction> instructions;
    instructions.clear();

    Instruction inst1("LOAD", 1, 0, 0, 1, 0);   // pc = 0
    Instruction inst2("ADDI", 2, 0, 0, 15, 0);  // pc = 1
    Instruction inst3("BEQ", 0, 1, 2, 3, 0);    // pc = 2 if branch 3+3 = 6
    Instruction inst4("ADDI", 0, 0, 0, 0, 0);   // pc = 3
    Instruction inst5("ADDI", 0, 0, 0, 0, 0);  // pc = 4
    Instruction inst6("ADDI", 0, 0, 0, 0, 0);   // pc = 5
    Instruction inst7("NAND", 3, 1, 2, 0, 0);   // pc = 6

    instructions.push_back(inst1);
    instructions.push_back(inst2);
    instructions.push_back(inst3);
    instructions.push_back(inst4);
    instructions.push_back(inst5);
    instructions.push_back(inst6);
    instructions.push_back(inst7);
//    instructions.push_back(inst3);
//    instructions.push_back(inst4);
//    instructions.push_back(inst5);



    vector <int16_t> memory (64*1024, 0);
    memory[1] = 15;
    memory[5] = 69;
    int startingPC = 0;
    int robCapacity = 6;
    unordered_map<string, int> stationCount;
    stationCount["ADD/ADDI"] = 3;
    stationCount["MUL"] = 2;
    stationCount["NAND"] = 1;
    stationCount["BEQ"] = 1;
    stationCount["LOAD"] = 1;
    stationCount["STORE"] = 1;
    stationCount["CALL/RET"] = 1;

    TomasuloSimulator simulator(instructions, memory, startingPC, robCapacity, stationCount);

    simulator.simulate();

}
