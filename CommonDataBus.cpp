//
// Created by Asus on 12/4/2024.
//

#include "CommonDataBus.h"

CommonDataBus::CommonDataBus() {
    value = 0;
    tag = -1;
}

void CommonDataBus::set(int16_t value, int tag) {
    this->value = value;
    this->tag = tag;
}

void CommonDataBus::clear() {
    value = 0;
    tag = -1;
}
