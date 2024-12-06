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
    string unit;
    string operation;
    int latency;
    int remainingCycles;
    int16_t operand1;
    int16_t operand2;
    int16_t A;
    bool busy;
    //ReservationStation *rs;

    FunctionalUnit(string name, string unit);
    bool isBusy();
    void execute();
    int getRemCycles();
    void startExec();
    void flush();
    int16_t getResult(int instPC, const int &PC);
    bool isEmpty();
};


#endif //TOMASULOSIMULATOR_FUNCTIONALUNIT_H
