//
// Created by Asus on 12/4/2024.
//

#include "ReorderBuffer.h"

ReorderBuffer::ReorderBuffer() {
    tag = -1;
    type = "";
    dest = 0;
    value = 0;
    ready = false;
}

ReorderBuffer::ReorderBuffer(int tag, string type, int dest, int actualPC): tag(tag), type(type), dest(dest), actualPC(actualPC) {
    value = 0;
    ready = false;
    remCycles = 0;
    if(type == "STORE")
        remCycles = 4;
}

void ReorderBuffer::setReady(int16_t value) {
    this->value = value;
    ready = true;
}
