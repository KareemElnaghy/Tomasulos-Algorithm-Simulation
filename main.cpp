#include <iostream>
#include "TomasuloSimulator.h"
using namespace std;


int main() {
    vector<Instruction> instructions;
    instructions.clear();

    Instruction inst1("ADDI", 1, 4, 0, 5, 0);
    Instruction inst2("MUL", 2, 5, 1, 0, 0);
    Instruction inst3("STORE", 0, 2, 0, 0, 0);

    instructions.push_back(inst1);
    instructions.push_back(inst2);
    instructions.push_back(inst3);

    unordered_map<int, uint16_t> memory;
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
