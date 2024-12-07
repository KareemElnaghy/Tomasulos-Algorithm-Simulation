//
// Created by Asus on 12/4/2024.
//

#include "CommonDataBus.h"

CommonDataBus::CommonDataBus() {
    value = 0;
    extraValue = 0;
    tag = -1;
    currBusy = false;
}

void CommonDataBus::writeToCDB(int16_t value, int tag) {
    this->value = value;
    this->extraValue = 0;
    this->tag = tag;
    currBusy = true;
}

void CommonDataBus::clear() {
    value = 0;
    extraValue = 0;
    tag = -1;
    currBusy = false;
}

bool CommonDataBus::isBusy() const {
    return currBusy;
}

void CommonDataBus::writeToCDB(int16_t value, int16_t extraValue, int tag) {
    this->value = value;
    this->extraValue = extraValue;
    this->tag = tag;
    currBusy = true;
}
