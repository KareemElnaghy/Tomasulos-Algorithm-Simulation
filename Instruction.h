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
    int16_t offset; // 5 bit offset
    int16_t label;  // 7 bit label

Instruction(string op, int rd, int rs1, int rs2, int16_t offset, int16_t label) {
    this->op = op;
    this->rd = rd;
    this->rs1 = rs1;
    this->rs2 = rs2;
    this->offset = (offset & 0x1F) | ((offset & 0x10) ? 0xFFE0 : 0x0000);
    this->label = (label & 0x7F) | ((label & 0x40) ? 0xFF80 : 0x0000);
}
};


#endif //TOMASULOSIMULATOR_INSTRUCTION_H
