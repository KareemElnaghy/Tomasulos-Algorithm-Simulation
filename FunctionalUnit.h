//
// Created by Asus on 12/4/2024.
//

#ifndef TOMASULOSIMULATOR_FUNCTIONALUNIT_H
#define TOMASULOSIMULATOR_FUNCTIONALUNIT_H
#include <string>
#include "ReservationStation.h"
using namespace std;

class FunctionalUnit {
public:
    string operation;
    int latency;
    int remainingCycles;
    ReservationStation *rs;

    FunctionalUnit(string op, int latency);
    bool isBusy();
    void execute();
    void startExec(ReservationStation *rs);
    void completeExec();
};


#endif //TOMASULOSIMULATOR_FUNCTIONALUNIT_H
