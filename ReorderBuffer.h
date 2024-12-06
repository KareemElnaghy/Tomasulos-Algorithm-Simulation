//
// Created by Asus on 12/4/2024.
//

#ifndef TOMASULOSIMULATOR_REORDERBUFFER_H
#define TOMASULOSIMULATOR_REORDERBUFFER_H
#include <string>
#include <cstdint>
using namespace std;

class ReorderBuffer {
public:
    int tag;
    string type;
    int dest;
    int16_t value;
    bool ready;
    int actualPC;

    ReorderBuffer();
    ReorderBuffer(int tag, string type, int dest, int actualPC);
    void setReady(int16_t value);
};


#endif //TOMASULOSIMULATOR_REORDERBUFFER_H
