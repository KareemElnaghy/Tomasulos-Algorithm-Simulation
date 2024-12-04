//
// Created by Asus on 12/4/2024.
//

#ifndef TOMASULOSIMULATOR_RESERVATIONSTATION_H
#define TOMASULOSIMULATOR_RESERVATIONSTATION_H
#include <string>
using namespace std;


class ReservationStation {
public:
    string name;    // name of the RS
    bool busy;      // is the RS busy
    string op;      // operation
    int Vj;     // value of operand j
    int Vk;     // value of operand k
    int Qj;     // reservation station holding the value of operand j
    int Qk;     // reservation station holding the value of operand k
    int A;      // immediate value
    int destination;    // destination register

    ReservationStation(string name);
    void clear();
    bool isBusy();


};


#endif //TOMASULOSIMULATOR_RESERVATIONSTATION_H
