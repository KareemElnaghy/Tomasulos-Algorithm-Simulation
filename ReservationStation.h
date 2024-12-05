//
// Created by Asus on 12/4/2024.
//

#ifndef TOMASULOSIMULATOR_RESERVATIONSTATION_H
#define TOMASULOSIMULATOR_RESERVATIONSTATION_H
#include <string>
#include "FunctionalUnit.h"
using namespace std;


class ReservationStation {
public:
    string name;    // name of the RS
    bool busy;      // is the RS busy
    string op;      // operation
    int16_t Vj;     // value of operand j
    int16_t Vk;     // value of operand k
    int Qj;     // reservation station holding the value of operand j
    int Qk;     // reservation station holding the value of operand k
    int16_t A;      // immediate value/address
    int destination;    // destination register
    int robTag;     // ROB tag
    enum Status  { EMPTY, ISSUED, EXECUTING, WRITING, COMMITTING };  // status of the RS;
    Status status = EMPTY;
    Status nextStatus = EMPTY;

    FunctionalUnit *fu; // functional unit executing the instruction

    ReservationStation(string name);
    void setFunctionalUnit(FunctionalUnit *fu);
    void clear();
    bool isBusy();
    bool isReady();
    bool issued();
    bool isExecuting();
    bool isWriting();
    bool isCommitting();
    void setNextStatus(Status status);
    void applyNextStatus();



};


#endif //TOMASULOSIMULATOR_RESERVATIONSTATION_H
