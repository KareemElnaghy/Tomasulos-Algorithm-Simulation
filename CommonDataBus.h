//
// Created by Asus on 12/4/2024.
//

#ifndef TOMASULOSIMULATOR_COMMONDATABUS_H
#define TOMASULOSIMULATOR_COMMONDATABUS_H

#include <cstdint>

class CommonDataBus {
public:
    int16_t value;
    int tag;
    bool busy;

    CommonDataBus();
    void writeToCDB(int16_t value, int tag);
    bool isBusy();
    void clear();



};


#endif //TOMASULOSIMULATOR_COMMONDATABUS_H
