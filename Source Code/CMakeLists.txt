cmake_minimum_required(VERSION 3.26)
project(TomasuloSimulator)

set(CMAKE_CXX_STANDARD 23)

add_executable(TomasuloSimulator main.cpp
        ReorderBuffer.h
        ReorderBuffer.cpp
        ReservationStation.h
        ReservationStation.cpp
        FunctionalUnit.h
        CommonDataBus.h
        TomasuloSimulator.h
        TomasuloSimulator.cpp
        FunctionalUnit.cpp
        CommonDataBus.cpp
)
