//
// Created by Asus on 12/4/2024.
//

#include "FunctionalUnit.h"

FunctionalUnit::FunctionalUnit(string op, int latency) {
    operation = op;
    this->latency = latency;
    rs = nullptr;
    remainingCycles = 0;
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
        rs->busy = false;
        rs->clear();
        rs = nullptr;
    }
}

