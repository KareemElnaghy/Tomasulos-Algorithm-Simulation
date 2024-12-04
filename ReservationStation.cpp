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


