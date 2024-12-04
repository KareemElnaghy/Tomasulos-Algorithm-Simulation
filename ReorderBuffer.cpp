//
// Created by Asus on 12/4/2024.
//

#include "ReorderBuffer.h"

ReorderBuffer::ReorderBuffer() {
    id = -1;
    type = "";
    dest = "";
    value = 0;
    ready = false;
}

ReorderBuffer::ReorderBuffer(int id, string type, string dest): id(id), type(type), dest(dest) {
    value = 0;
    ready = false;
}

void ReorderBuffer::setReady(int16_t value) {
    this->value = value;
    ready = true;
}
