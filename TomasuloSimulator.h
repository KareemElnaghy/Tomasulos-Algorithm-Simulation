//
// Created by Asus on 12/4/2024.
//

#ifndef TOMASULOSIMULATOR_TOMASULOSIMULATOR_H
#define TOMASULOSIMULATOR_TOMASULOSIMULATOR_H
#include <iostream>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include "Instruction.h"
#include "ReorderBuffer.h"
#include "FunctionalUnit.h"
#include "CommonDataBus.h"
#include "ReservationStation.h"
using namespace std;


class TomasuloSimulator {
private:
    vector<Instruction> instructions;
    unordered_map<int, uint16_t> memory;
    vector<int> registers;
    int PC;
    vector <ReorderBuffer> rob;
    vector<ReservationStation> rsList;
    vector<FunctionalUnit> fuList;
    CommonDataBus cdb;
    int totalCycles;
    int robCapacity;

public:
    TomasuloSimulator();
    TomasuloSimulator(vector<Instruction> instructions, unordered_map<int, uint16_t> memory, int startingPC, int robCapacity, unordered_map<string, int> stationCount);

    void simulate();

private:
    void issue();
    void execute();
    void write();
    void commit();
    void rollback();
    void printState();
    bool hasFreeRS(Instruction inst);

};


#endif //TOMASULOSIMULATOR_TOMASULOSIMULATOR_H
