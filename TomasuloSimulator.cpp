//
// Created by Asus on 12/4/2024.
//

#include "TomasuloSimulator.h"

TomasuloSimulator::TomasuloSimulator() {
    PC = 0;
    totalCycles = 0;
}

TomasuloSimulator::TomasuloSimulator(vector<Instruction> instructions, unordered_map<int, uint16_t> memory, int startingPC, int robCapacity, unordered_map<string, int> stationCount)
: instructions(instructions), memory(memory), PC(startingPC), robCapacity(robCapacity)
{
    totalCycles = 0;
    registers.resize(8, 0);
    rob.resize(robCapacity);

    for(auto &p: stationCount) {
        for(int i=0; i<p.second; i++) {
            rsList.emplace_back(p.first);
        }
    }

}

void TomasuloSimulator::simulate() {
    while(PC<instructions.size() || !rob.empty() || !rsList.empty() || !fuList.empty()) {
        issue();
        execute();
        write();
        commit();
        printState();
        totalCycles++;
    }


}

void TomasuloSimulator::issue() {
    if (PC >= instructions.size()) return;

    Instruction inst = instructions[PC];

    if(rob.size() >= robCapacity || !hasFreeRS(inst)) return;   // if ROB is full or no free RS, stall

    for(auto &rs: rsList){  // find a free RS, issue the instruction, and increment PC
        if(!rs.isBusy() && rs.op == inst.op) {
            rs.busy = true;
            rs.op = inst.op;
            rs.destination = inst.rd;
            rs.A = inst.offset;
            rs.Qj = rs.Qk = 0;

            if(inst.rs1 != -1) {
                rs.Vj = registers[inst.rs1];
                rs.Qj = 0;
            } else {
                rs.Vj = 0;
                rs.Qj = inst.rs1;
            }

            if(inst.rs2 != -1) {
                rs.Vk = registers[inst.rs2];
                rs.Qk = 0;
            } else {
                rs.Vk = 0;
                rs.Qk = inst.rs2;
            }
            PC++;
            break;
        }
    }

}

bool TomasuloSimulator::hasFreeRS(Instruction inst) {
    for(auto &rs: rsList) {
        if(!rs.isBusy() && rs.op == inst.op) {
            return true;
        }
    }
    return false;
}

void TomasuloSimulator::execute() {

}

void TomasuloSimulator::write() {

}

void TomasuloSimulator::commit() {

}

void TomasuloSimulator::rollback() {

}

void TomasuloSimulator::printState() {

}

