//
// Created by Asus on 12/4/2024.
//

#include "FunctionalUnit.h"

FunctionalUnit::FunctionalUnit(string name, string unit, int lat): name(name), unit(unit), remainingCycles(0), operand1(0), operand2(0), A(0) {
    operation = "";
    if(name == "ADD/ADDI")
        latency = lat;
    else if(name == "MUL")
        latency = lat;
    else if(name == "NAND")
        latency = lat;
    else if(name == "BEQ")
        latency = lat;
    else if(name == "LOAD")
        latency = lat;
    else if(name == "STORE")
        latency = lat;
    else if(name == "CALL/RET")
        latency = lat;
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
    busy = false;
}

int FunctionalUnit::getRemCycles() {
    return remainingCycles;
}

int16_t FunctionalUnit::getResult(int instPC,  const int &PC) {
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
        return (operand1 == operand2) ? (instPC + 1 + A) : PC+1; // if equal, branch to A, else go to next instruction
    }
    else if (operation == "LOAD") {
        return operand1 + A;
    }
    else if (operation == "STORE") {
        return operand2 + A;
    }
    else if (operation == "CALL") {
        return A;
    }
    else if (operation == "RET") {
        return operand1;
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
