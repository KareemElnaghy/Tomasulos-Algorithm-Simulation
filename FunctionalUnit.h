//
// Created by Asus on 12/4/2024.
//

#ifndef TOMASULOSIMULATOR_FUNCTIONALUNIT_H
#define TOMASULOSIMULATOR_FUNCTIONALUNIT_H
#include <string>
#include <iostream>
#include <cstdint>
#include "ReservationStation.h"
using namespace std;

class FunctionalUnit {
public:
    string name;
    string operation;
    int latency;
    int remainingCycles;
    ReservationStation *rs;

    FunctionalUnit(string name);
    bool isBusy();
    void execute();
    int getRemCycles();
    void startExec(ReservationStation *rs);
    void completeExec();
    int16_t getResult(const int &PC);
};


#endif //TOMASULOSIMULATOR_FUNCTIONALUNIT_H
