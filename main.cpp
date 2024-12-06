#include <iostream>
#include "TomasuloSimulator.h"
using namespace std;


int main() {
    vector<Instruction> instructions;
    instructions.clear();

    Instruction inst5("LOAD", 1, 0, 0, 1, 0);
    Instruction inst1("ADDI", 2, 0, 0, 5, 0);
    Instruction inst2("ADD", 3, 1, 2, 0, 0);
    Instruction inst3("MUL", 4, 3, 2, 0, 0);
    Instruction inst4("STORE", 0, 4, 0, 0, 0);


    instructions.push_back(inst5);
    instructions.push_back(inst1);
    instructions.push_back(inst2);
    instructions.push_back(inst3);
    instructions.push_back(inst4);


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
