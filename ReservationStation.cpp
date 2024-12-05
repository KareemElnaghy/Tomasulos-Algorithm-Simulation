//
// Created by Asus on 12/4/2024.
//

#include "ReservationStation.h"

ReservationStation::ReservationStation(string name): busy(false), op(""), Vj(0), Vk(0), Qj(0), Qk(0), A(0), destination(0) {
    this->name = name;
}

void ReservationStation::clear() {
    busy = false;
    op = "";
    Vj = 0;
    Vk = 0;
    Qj = 0;
    Qk = 0;
    A = 0;
    destination = 0;
}

bool ReservationStation::isBusy() {
    return busy;
}

bool ReservationStation::isReady() {
    return (Qj == 0 && Qk == 0);
}

bool ReservationStation::isExecuting() {
    if(status == EXECUTING) {
        return true;
    }
    return false;
}

bool ReservationStation::isWriting() {
    if(status == WRITING) {
        return true;
    }
    return false;
}

bool ReservationStation::isCommitting() {
    if(status == COMMITTING) {
        return true;
    }
    return false;
}

bool ReservationStation::issued() {
    if(status == ISSUED) {
        return true;
    }
    return false;
}

void ReservationStation::setFunctionalUnit(FunctionalUnit *fu) {
    this->fu = fu;
}

void ReservationStation::setNextStatus(ReservationStation::Status status) {
    nextStatus = status;
}

void ReservationStation::applyNextStatus() {
    if(nextStatus != EMPTY) {
        status = nextStatus;
        nextStatus = EMPTY;
    }
}


