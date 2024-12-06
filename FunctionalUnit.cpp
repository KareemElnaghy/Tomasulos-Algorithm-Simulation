//
// Created by Asus on 12/4/2024.
//

#include "FunctionalUnit.h"

FunctionalUnit::FunctionalUnit(string name, string unit): name(name), unit(unit), remainingCycles(0), operand1(0), operand2(0), A(0) {
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
    return (busy);
}

void FunctionalUnit::execute() {
    if(remainingCycles>0) {
        remainingCycles--;
    }
}

void FunctionalUnit::startExec() {
    remainingCycles = latency;
    busy = true;
}

void FunctionalUnit::flush() {
    operation = "";
    remainingCycles = 0;
    operand1 = 0;
    operand2 = 0;
    A = 0;
    //rs = nullptr;
    busy = false;
}

int FunctionalUnit::getRemCycles() {
    return remainingCycles;
}

int16_t FunctionalUnit::getResult(int instPC, const int &PC) {
    if (operation == "ADD") {
        return operand1 + operand2;
    } else if (operation == "ADDI") {
        return operand1 + A;
    } else if (operation == "MUL") {
        return operand1 * operand2;
    } else if (operation == "NAND") {
        return ~(operand1 & operand2);
    }
    else if (operation == "BEQ") {
        return (operand1 == operand2) ? (instPC + 1 + A) : PC; // if equal, branch to A, else go to next instruction
    }
    else if (operation == "LOAD") {
        return operand1 + A;
    }
    else if (operation == "STORE") {
        return operand2 + A;
    }
    else if (operation == "CALL") {    // TODO: do call later
        return 0;
    }
    else if (operation == "RET") { // TODO: do ret later
        return 0;
    }
    else {
        return 0;
    }
}

bool FunctionalUnit::isEmpty() {
    if(operation == "" && remainingCycles == 0) {
        return true;
    }
    return false;
}
