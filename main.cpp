#include <iostream>
#include "TomasuloSimulator.h"
using namespace std;


int main() {
    vector<Instruction> instructions = {
        {"ADDI", 1, 2, 0, 5, 0},
        {"ADD", 2, 1, 1, 0, 0}};

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
