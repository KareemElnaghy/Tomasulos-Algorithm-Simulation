//
// Created by Asus on 12/4/2024.
//

#include "TomasuloSimulator.h"

TomasuloSimulator::TomasuloSimulator() {
    PC = 0;
    totalCycles = 0;
    robCapacity = 0;
    fuResult = 0;
}

TomasuloSimulator::TomasuloSimulator(vector<Instruction> instructions, unordered_map<int, uint16_t> memory, int startingPC, int robCapacity, unordered_map<string, int> stationCount)
: instructions(instructions), memory(memory), PC(startingPC), robCapacity(robCapacity)
{
    totalCycles = 0;
    registers.resize(8, 0);
    fuResult = 0;
    tags.resize(8, false);

    for(auto &p: stationCount) {
        for(int i=0; i<p.second; i++) {
            rsList.emplace_back(p.first);
            FunctionalUnit fu(p.first);
            fuList.emplace_back(p.first);
        }
    }

}

void TomasuloSimulator::simulate() {
    while(PC<instructions.size() || !rob.empty() || !rsList.empty() || !fuList.empty()) {
        advanceCycle();
        issue();
        execute();
        write();
        commit();
        printState();
        totalCycles++;
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

void TomasuloSimulator::issue() {
    if (PC >= instructions.size()) return;

    Instruction inst = instructions[PC];

    if(rob.size() >= robCapacity || !hasFreeRS(inst)) return;   // if ROB is full or no free RS, stall

    for(auto &rs: rsList){  // find a free RS, issue the instruction, and increment PC
        if(!rs.isBusy() && rs.op == inst.op) {
            int tag = getTag();
            ReorderBuffer robEntry(tag,inst.op, inst.rd);
            rob.push(robEntry);
            rs.setNextStatus(ReservationStation::ISSUED);
            rs.robTag = tag;
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

void TomasuloSimulator::execute() {
    for(auto &rs: rsList) {
        if(rs.isBusy() && rs.isReady() && rs.issued()) {    // if RS is busy, ready, and issued
                rs.setNextStatus(ReservationStation::EXECUTING);

                for(auto &fu: fuList) {
                    if(fu.operation == rs.op && !fu.isBusy()) {
                        rs.setFunctionalUnit(fu);
                        fu.startExec(&rs);
                        break;
                    }
                }
            }
        if(rs.isExecuting() && rs.fu->getRemCycles() == 0 && !cdb.isBusy()) { // if RS is executing and FU is done
            fuResult = rs.fu->getResult(PC);
            cdb.writeToCDB(fuResult, rs.robTag);
            rs.setNextStatus(ReservationStation::WRITING);
        }

        if(rs.isExecuting() && rs.fu->getRemCycles() > 0) { // if RS is executing and FU is not done
            rs.fu->execute();
        }


    }


}


void TomasuloSimulator::write() {
    for(auto &rs: rsList) { // if RS is writing, update ROB entry and clear RS
        if(rs.isWriting()) {
            updateROBEntry(rs.robTag, cdb.value, rob);
            rs.setNextStatus(ReservationStation::COMMITTING);
            rs.clear();
        }
    }
    
}

void TomasuloSimulator::commit() {
    if(!rob.empty() && rob.front().ready) {
        ReorderBuffer entry = rob.front();
        rob.pop();
        registers[entry.dest] = entry.value;
        tags[entry.tag] = false;
    }


}

void TomasuloSimulator::rollback() {

}

void TomasuloSimulator::printState() {

}

void TomasuloSimulator::advanceCycle() {
    for(auto &rs: rsList) {
        rs.applyNextStatus();
    }
}

int TomasuloSimulator::getTag() {
    for(int i=0; i<tags.size(); i++) {
        if(!tags[i]) {
            tags[i] = true;
            return i;
        }
    }
    return -1;
}

void TomasuloSimulator::updateROBEntry(int tag, int16_t value, queue<ReorderBuffer> &rob) {
    queue<ReorderBuffer> temp;
    while(!rob.empty()) {
        ReorderBuffer entry = rob.front();
        rob.pop();
        if(entry.tag == tag) {
            entry.setReady(value);
        }
        temp.push(entry);
    }
    rob = temp;

}



