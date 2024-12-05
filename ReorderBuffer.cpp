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

ReorderBuffer::ReorderBuffer(int id, string type, int dest): tag(tag), type(type), dest(dest) {
    value = 0;
    ready = false;
}

void ReorderBuffer::setReady(int16_t value) {
    this->value = value;
    ready = true;
}
