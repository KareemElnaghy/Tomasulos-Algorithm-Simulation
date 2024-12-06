//
// Created by Asus on 12/4/2024.
//

#include "ReservationStation.h"

ReservationStation::ReservationStation(string name, string unit): busy(false), op(""), Vj(0), Vk(0), Qj(0), Qk(0), A(0), destination(0) {
    this->name = name;
    this->unit = unit;

    remCyclesLoad = 4;
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
    readyToWrite = false;
    remCyclesLoad = 4;

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
    if ((nextStatus == EMPTY && status == WRITING) || nextStatus != EMPTY) {
        status = nextStatus;
        nextStatus = EMPTY;
    }
}

bool ReservationStation::isEmpty() {
    return (status == EMPTY);
}

string ReservationStation::getStatus() {
    if(status == EMPTY) {
        return "EMPTY";
    } else if(status == ISSUED) {
        return "ISSUED";
    } else if(status == EXECUTING) {
        return "EXECUTING";
    } else if(status == WRITING) {
        return "WRITING";
    } else if(status == COMMITTING) {
        return "COMMITTING";
    }
    return "";
}

string ReservationStation::getNextStatus() {
    if(nextStatus == EMPTY) {
        return "EMPTY";
    } else if(nextStatus == ISSUED) {
        return "ISSUED";
    } else if(nextStatus == EXECUTING) {
        return "EXECUTING";
    } else if(nextStatus == WRITING) {
        return "WRITING";
    } else if(nextStatus == COMMITTING) {
        return "COMMITTING";
    }
    return "";
}

bool ReservationStation::isReadyToWrite() {
    return readyToWrite;
}


