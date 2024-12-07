#include <iostream>
#include "TomasuloSimulator.h"
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;


vector<Instruction> parseInstructions(const string &filename) {
    vector<Instruction> instructions;
    ifstream file(filename);
    string line;

    if (!file.is_open()) {
        throw runtime_error("Could not open file");
    }

    while (getline(file, line)) {
        istringstream iss(line);
        string op, rd_str, rs1_str, rs2_str, addr_str;
        int rd = 0, rs1 = 0, rs2 = 0;
        int16_t offset = 0, label = 0;

        iss >> op;

            if (op == "LOAD") {
                char offsetChar;
                iss >> rd_str >> offsetChar >> addr_str;
                if (!rd_str.empty() && !addr_str.empty()) {
                    rd = stoi(rd_str.substr(1));
                    rs1 = stoi(addr_str.substr(2));
                    offset = offsetChar - '0';
                }
            }
            else if (op == "STORE") {
                char offsetChar;
                iss >> rs1_str >> offsetChar >> addr_str;

                if (!rs1_str.empty() && !addr_str.empty()) {
                    rs1 = stoi(rs1_str.substr(1));
                    rs2 = stoi(addr_str.substr(2));
                    offset = offsetChar - '0';
                }
            }
            else if (op == "ADDI") {
                    iss >> rd_str >> rs1_str >> addr_str;
                    if (!rd_str.empty() && !rs1_str.empty() && !addr_str.empty()) {
                        rd = stoi(rd_str.substr(1));
                        rs1 = stoi(rs1_str.substr(1));
                        offset = stoi(addr_str);
                    }
                }
            else if (op == "BEQ") {
                iss >> rs1_str >> rs2_str >> addr_str;
                if (!rs1_str.empty() && !rs2_str.empty() && !addr_str.empty()) {
                    rs1 = stoi(rs1_str.substr(1));
                    rs2 = stoi(rs2_str.substr(1));
                    offset = stoi(addr_str);
                }
            }
            else {
                iss >> rd_str >> rs1_str >> rs2_str;
                if (!rd_str.empty() && !rs1_str.empty() && !rs2_str.empty()) {
                    rd = stoi(rd_str.substr(1));
                    rs1 = stoi(rs1_str.substr(1));
                    rs2 = stoi(rs2_str.substr(1));
                }
            }

        instructions.emplace_back(op, rd, rs1, rs2, offset, label);
    }

    file.close();
    return instructions;
}

int main() {
    vector<Instruction> instructions;
    instructions.clear();
    instructions = parseInstructions("C:\\Users\\Asus\\CLionProjects\\Tomasulos-Algorithm-Simulation\\instruction.txt");




//    Instruction inst1("LOAD", 1, 0, 0, 1, 0);   // pc = 0
//    Instruction inst2("ADDI", 2, 0, 0, 15, 0);  // pc = 1
//    Instruction inst3("BEQ", 0, 1, 2, 3, 0);    // pc = 2 if branch 3+3 = 6
//    Instruction inst4("ADDI", 0, 0, 0, 0, 0);   // pc = 3
//    Instruction inst5("ADDI", 7, 0, 0, -7, 0);   // pc = 4
//    Instruction inst6("NAND", 3, 1, 2, 0, 0);   // pc = 5
//    Instruction inst7("MUL", 7, 1, 2, 0, 0);   // pc = 6
//    Instruction inst8("ADDI", 0, 0, 0, 0, 0);  // pc = 7
//    Instruction inst9("ADDI", 0, 0, 0, 0, 0);   // pc = 8
//
//    instructions.push_back(inst1);
//    instructions.push_back(inst2);
//    instructions.push_back(inst3);
//    instructions.push_back(inst4);
//    instructions.push_back(inst5);
//    instructions.push_back(inst6);
//    instructions.push_back(inst7);
//    instructions.push_back(inst8);
//    instructions.push_back(inst9);



    vector <int16_t> memory (64*1024, 0);
    memory[1] = 15;
    memory[5] = 69;
    int startingPC = 0;
    int robCapacity = 6;
    unordered_map<string, int> stationCount;
    stationCount["ADD/ADDI"] = 3;
    stationCount["MUL"] = 2;
    stationCount["NAND"] = 1;
    stationCount["BEQ"] = 1;
    stationCount["LOAD"] = 1;
    stationCount["STORE"] = 1;
    stationCount["CALL/RET"] = 1;

    TomasuloSimulator simulator(instructions, memory, startingPC, robCapacity, stationCount);

    simulator.simulate();

}
