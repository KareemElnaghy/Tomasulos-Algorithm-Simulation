//
// Created by Asus on 12/4/2024.
//

#ifndef TOMASULOSIMULATOR_INSTRUCTION_H
#define TOMASULOSIMULATOR_INSTRUCTION_H
#include <string>
using namespace std;

class Instruction {
public:
    string op;
    int rd, rs1, rs2;
    int16_t offset;
    int16_t label;

};


#endif //TOMASULOSIMULATOR_INSTRUCTION_H
