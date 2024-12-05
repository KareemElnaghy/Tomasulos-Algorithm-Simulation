//
// Created by Asus on 12/4/2024.
//

#include "CommonDataBus.h"

CommonDataBus::CommonDataBus() {
    value = 0;
    tag = -1;
}

void CommonDataBus::writeToCDB(int16_t value, int tag) {
    this->value = value;
    this->tag = tag;
    busy = true;
}

void CommonDataBus::clear() {
    value = 0;
    tag = -1;
    busy = false;
}

bool CommonDataBus::isBusy() {
    return busy;
}
