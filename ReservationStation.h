//
// Created by Asus on 12/4/2024.
//

#ifndef TOMASULOSIMULATOR_RESERVATIONSTATION_H
#define TOMASULOSIMULATOR_RESERVATIONSTATION_H
#include <string>
#include "FunctionalUnit.h"
class FunctionalUnit;
using namespace std;


class ReservationStation {
public:
    string name;    // name of the RS
    string unit;      // unit number
    bool busy;      // is the RS busy
    string op;      // operation
    int reg1;
    int reg2;
    int16_t Vj;     // value of operand j
    int16_t Vk;     // value of operand k
    int Qj;     // reservation station holding the value of operand j
    int Qk;     // reservation station holding the value of operand k
    int16_t A;      // immediate value/address
    int destination;    // destination register
    int robTag;     // ROB tag
    enum Status  { EMPTY, ISSUED, EXECUTING, WRITING, COMMITTING};  // status of the RS;
    Status status = EMPTY;
    Status nextStatus = EMPTY;
    int instPC;     // PC of the instruction
    bool readyToWrite = false;
    FunctionalUnit *fu; // functional unit executing the instruction
    int remCyclesLoad;

    ReservationStation(string name, string unit);
    void setFunctionalUnit(FunctionalUnit *fu);
    void clear();
    bool isBusy();
    bool isReady();
    bool issued();
    bool isExecuting();
    bool isWriting();
    bool isCommitting();
    bool isReadyToWrite();
    void setNextStatus(Status status);
    void applyNextStatus();
    bool isEmpty();
    string getStatus();
    string getNextStatus();


};


#endif //TOMASULOSIMULATOR_RESERVATIONSTATION_H
