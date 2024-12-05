//
// Created by Asus on 12/4/2024.
//

#include "FunctionalUnit.h"

FunctionalUnit::FunctionalUnit(string name): name(name) {
    operation = "";
    if(name == "ADD/ADDI")
        latency = 2;
    else if(name == "MUL")
        latency = 8;
    else if(name == "NAND")
        latency = 1;
    else if(name == "BEQ")
        latency = 1;
    else if(name == "LOAD")
        latency = 2;
    else if(name == "STORE")
        latency = 2;
    else if(name == "CALL/RET")
        latency = 1;
    else
        latency = 0;
}

bool FunctionalUnit::isBusy() {
    return (remainingCycles>0);
}

void FunctionalUnit::execute() {
    if(remainingCycles>0) {
        remainingCycles--;
    }
}

void FunctionalUnit::startExec(ReservationStation *station) {
    this->rs = station;
    remainingCycles = latency;
}

void FunctionalUnit::completeExec() {
    if(rs!=nullptr) {
        rs->clear();
    }
}

int FunctionalUnit::getRemCycles() {
    return remainingCycles;
}

int16_t FunctionalUnit::getResult(const int &PC) {
    if (rs->op == "ADD") {
        return rs->Vj + rs->Vk;
    } else if (rs->op == "ADDI") {
        return rs->Vj + rs->A;
    } else if (rs->op == "MUL") {
        return rs->Vj * rs->Vk;
    } else if (rs->op == "NAND") {
        return ~(rs->Vj & rs->Vk);
    }
    else if (rs->op == "BEQ") {
        return (rs->Vj == rs->Vk) ? (rs->A + PC) : PC+1;    // Do branch later
    }
    else if (rs->op == "LOAD") {
        return rs->Vj + rs->A;
    }
    else if (rs->op == "STORE") {
        return rs->Vj + rs->A;
    }
    else if (rs->op == "CALL") {    // do call later
        return rs->A;
    }
    else if (rs->op == "RET") { // do ret later
        return rs->A;
    }
    else {
        return 0;
    }
}
