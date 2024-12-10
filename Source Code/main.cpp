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
            else if (op == "CALL"){
                iss >> label;
                rd = 1;
            }
            else if (op == "RET") {
                rs1 = 1;
            }
            else if(op == "ADD" || op == "MUL" || op == "NAND") {
                iss >> rd_str >> rs1_str >> rs2_str;
                if (!rd_str.empty() && !rs1_str.empty() && !rs2_str.empty()) {
                    rd = stoi(rd_str.substr(1));
                    rs1 = stoi(rs1_str.substr(1));
                    rs2 = stoi(rs2_str.substr(1));
                }
            }
            else
            {
                op = "ADDI";
                rd = 0;
                rs1 = 0;
                rs2 = 0;
                offset = 0;
                label = 0;
            }

        instructions.emplace_back(op, rd, rs1, rs2, offset, label);
    }

    file.close();
    return instructions;
}

int main() {
    vector<Instruction> instructions;
    instructions.clear();
    instructions = parseInstructions("C:\\Users\\Asus\\CLionProjects\\Tomasulos-Algorithm-Simulation\\testcase.txt");

    vector <int16_t> memory (64*1024, 0);
    memory[0] = 10;
    memory[1] = 15;
    memory[2] = 30;
    int startingPC = 0;
    int robCapacity = 0;
    unordered_map<string, int> stationCount;
    unordered_map<string, int> latency;

    // User input
    cout<<"Starting PC: ";
    cin>>startingPC;

    cout<<"Number of ROB entries: ";
    cin>>robCapacity;   // 6

    cout<<"Enter the number of ADD/ADDI stations and Latency: ";
    cin>>stationCount["ADD/ADDI"];  // 3
    cin>>latency["ADD/ADDI"];   // 2

    cout<<"Enter the number of MUL stations and Latency: ";
    cin>>stationCount["MUL"];   // 2
    cin>>latency["MUL"];        // 8

    cout<<"Enter the number of NAND stations and Latency: ";
    cin>>stationCount["NAND"];  // 1
    cin>>latency["NAND"];       // 1

    cout<<"Enter the number of BEQ stations and Latency: ";
    cin>>stationCount["BEQ"];       // 1
    cin>>latency["BEQ"];        // 1

    cout<<"Enter the number of LOAD stations and Latency: ";
    cin>>stationCount["LOAD"];      // 2
    cin>>latency["LOAD"];       // 2

    cout<<"Enter the number of STORE stations and Latency: ";
    cin>>stationCount["STORE"];     // 1
    cin>>latency["STORE"];      // 2

    cout<<"Enter the number of CALL/RET stations and Latency: ";
    cin>>stationCount["CALL/RET"];      // 1
    cin>>latency["CALL/RET"];       // 1

//    stationCount["ADD/ADDI"] = 3;
//    stationCount["MUL"] = 2;
//    stationCount["NAND"] = 1;
//    stationCount["BEQ"] = 1;
//    stationCount["LOAD"] = 2;
//    stationCount["STORE"] = 1;
//    stationCount["CALL/RET"] = 1;
//
//    latency["ADD/ADDI"] = 2;
//    latency["MUL"] = 8;
//    latency["NAND"] = 1;
//    latency["BEQ"] = 1;
//    latency["LOAD"] = 2;
//    latency["STORE"] = 2;
//    latency["CALL/RET"] = 1;

    TomasuloSimulator simulator(instructions, memory, startingPC, robCapacity, stationCount, latency);

    simulator.simulate();


}
