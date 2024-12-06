//
// Created by Asus on 12/4/2024.
//

#include "TomasuloSimulator.h"

TomasuloSimulator::TomasuloSimulator() {
    PC = 0;
    totalCycles = 0;
    robCapacity = 0;
    fuResult = 0;
}

void printROB(ReorderBuffer rob)
{
    cout << "Tag: " << rob.tag << endl;
    cout << "Type: " << rob.type << endl;
    cout << "Destination: " << rob.dest << endl;
    cout << "Value: " << rob.value << endl;
    cout << "Ready: " << rob.ready << endl;
    cout << "Actual PC: " << rob.actualPC << endl;
    cout << endl;
}
void printReservationStation(ReservationStation rs)
{
    cout << "Name: " << rs.name + rs.unit << endl;
    cout << "Busy: " << rs.busy << endl;
    cout << "Operation: " << rs.op << endl;
    cout << "Vj: " << rs.Vj << endl;
    cout << "Vk: " << rs.Vk << endl;
    cout << "Qj: " << rs.Qj << endl;
    cout << "Qk: " << rs.Qk << endl;
    cout << "A: " << rs.A << endl;
    cout << "Destination: " << rs.destination << endl;
    cout << "ROB Tag: " << rs.robTag << endl;
    cout << "Status: " << rs.getStatus() << endl;
    cout << "Next Status: " << rs.getNextStatus() << endl;
    cout << "Instruction PC: " << rs.instPC << endl;
    if(rs.fu != nullptr) {
        cout << "Functional Unit: " << rs.fu->name + rs.fu->unit << endl;
    }
    else {
        cout << "Functional Unit: None" << endl;
    }
    cout << endl;
}

void printFU(FunctionalUnit fu)
{
    cout << "Name: " << fu.name + fu.unit << endl;
    cout << "Operation: " << fu.operation << endl;
    cout << "Latency: " << fu.latency << endl;
    cout << "Remaining Cycles: " << fu.remainingCycles << endl;
    cout << "Operand 1: " << fu.operand1 << endl;
    cout << "Operand 2: " << fu.operand2 << endl;
    cout << "A: " << fu.A << endl;
    cout << endl;
}

void printRegisters(vector<int> registers)
{
    cout << "Registers: ";
    for (int i = 0; i < 8; i++) {
        cout << "R" << i << ": " << registers[i] << " ";
    }
    cout << endl;
}

TomasuloSimulator::TomasuloSimulator(vector<Instruction> instructions, unordered_map<int, uint16_t> memory, int startingPC, int robCapacity, unordered_map<string, int> stationCount)
: instructions(instructions), memory(memory), PC(startingPC), robCapacity(robCapacity)
{
    totalCycles = 0;
    registers.resize(8, 0); // TODO: hard wire register 0 to be zero
    destRegs.resize(8, 0);
    robRegTable.resize(8, 0);
    fuResult = 0;
    tags.resize(8, false);

    for(auto &p: stationCount) {    // TODO: handle naming issue
        for(int i=0; i<p.second; i++) {
            rsList.emplace_back(p.first, to_string(i+1));
            FunctionalUnit fu(p.first, to_string(i+1));
            fuList.emplace_back(p.first, to_string(i+1));
        }
    }

}

void TomasuloSimulator::simulate() {
    // TODO: Implement all the various counters
    // FIXME: Reservation Table Info displays a cycle early (I think)
    // FIXME: Execution should start when the operands are ready AND prev instructions are done broadcasting to CDB
    int counter = 0;
    while (PC < instructions.size() || !rob.empty() || !isRSListEmpty() || !isFUListEmpty()) {
        cout<<"=====================================CYCLE "<<totalCycles<<"========================================"<<endl;
        advanceCycle(); // Advance the cycle
        issue();        // Issue the instruction
        execute();      // Execute the instruction
        write();        // Write the result to the CDB
        commit();       // Commit the result to the register
        // printState(); // Print the state of the simulator
        totalCycles++;  // Increment the cycle count
        counter++;      // for debugging

//        cout<<"PC Condition "<<(PC<instructions.size())<<endl;
//        cout<<"ROB Condition "<<(!rob.empty())<<endl;
//        cout<<"RS Condition "<<(!isRSListEmpty())<<endl;
//        cout<<"FU Condition "<<(!isFUListEmpty())<<endl;
        cout<<"PC: "<<PC<<endl;
        for (auto &rs : rsList) {
            if (rs.name == "ADD/ADDI" && ((rs.unit == "1") || (rs.unit == "2"))) {
                printReservationStation(rs);
            }
        }

        //printRegisters(destRegs);
        if (counter == 15) {
            break;
        }
    }

    //printRegisters(registers);
}






bool TomasuloSimulator::hasFreeRS(Instruction inst) {   // Check if there is a free RS for the instruction
    for (auto &rs : rsList) {
        if (!rs.isBusy() && rs.name.find(inst.op) != string::npos) {
            return true;
        }
    }
    return false;
}

void TomasuloSimulator::issue() {
    if (PC >= instructions.size()) return;  // If PC is out of bounds, return

    Instruction inst = instructions[PC];

    cout<<"Instruction: "<<inst.op<<endl;
    // If ROB is full or no free RS, stall
    if (rob.size() >= robCapacity || !hasFreeRS(inst)) return;


    // Find a free RS, issue the instruction, and increment PC
    for (auto &rs : rsList) {
        if (rs.issued()) { // If RS is not busy and operation matches
            PC++;   // Increment PC
            int tag = getTag();
            ReorderBuffer robEntry(tag, inst.op, inst.rd, PC+1);  // Create a new ROB entry
            rob.push(robEntry); // Push the ROB entry to the ROB
//            rs.setNextStatus(ReservationStation::ISSUED);
            rs.robTag = tag;
            destRegs[inst.rd] = tag;
            robRegTable[inst.rd] = tag;
            rs.busy = true;
            rs.op = inst.op;
            rs.destination = inst.rd;
            rs.A = inst.offset;
            rs.instPC = PC;
            rs.reg1 = inst.rs1;
            rs.reg2 = inst.rs2;

            // Check if rs1 is ready
            rs.Qj = destRegs[inst.rs1]; // FIXME: Check rs1/rs2 if its -1 first

            // Check if rs2 is ready
            rs.Qk = destRegs[inst.rs2];

            // Set Vj and Vk
            if (inst.rs1 != -1) {   // If rs1 is not -1, set Vj and Qj
                rs.Vj = registers[inst.rs1];
            } else {
                rs.Vj = 0;
            }

            if (inst.rs2 != -1) {   // If rs2 is not -1, set Vk and Qk
                rs.Vk = registers[inst.rs2];
            } else {
                rs.Vk = 0;
            }

            break;
        }
    }


    for(auto &rs: rsList) {
//        cout<<"RS Name: "<<rs.name<<endl;
//        cout<<"RS Not Busy: "<<!rs.isBusy()<<endl;
//        cout<<"RS Operation: "<<(rs.name.find(inst.op) != string::npos)<<endl;
        if (!rs.isBusy() && (rs.name.find(inst.op) != string::npos) )
        {
            cout<<"DEBUG"<<endl;

            rs.setNextStatus(ReservationStation::ISSUED);
            //printReservationStation(rs);
            break;
        }
    }
}

void TomasuloSimulator::execute() {
    for (auto &rs : rsList) {
        // If RS is busy, ready, and issued

        if (rs.isBusy() && rs.isReady() && rs.issued()) {
            rs.setNextStatus(ReservationStation::EXECUTING);

            for (auto &fu : fuList) {   // Find a free FU and start execution
                if (fu.name.find(rs.op) != string::npos && !fu.isBusy()) {
                    fu.operation = rs.op;
                    fu.operand1 = rs.Vj;
                    fu.operand2 = rs.Vk;
                    fu.A = rs.A;
                    FunctionalUnit *fuPointer = &fu;
                    rs.setFunctionalUnit(fuPointer);

                    fu.startExec();
                    break;
                }
            }
        }


        // If RS is executing and FU is not done
        if (rs.isExecuting() && rs.fu->getRemCycles() > 0) {
            rs.setNextStatus(ReservationStation::EXECUTING);
            rs.fu->execute();
        }

        // If RS is executing and FU is done
        if (rs.isExecuting() && rs.fu->getRemCycles() == 0 && !cdb.isBusy()) {
            fuResult = rs.fu->getResult(rs.instPC, PC);
            rs.setNextStatus(ReservationStation::WRITING);
            cdb.writeToCDB(fuResult, rs.robTag);


            cout<<"FU Result: "<<fuResult<<endl;

        }

    }
}

void TomasuloSimulator::write() {
    for (auto &rs : rsList) {
        // If RS is writing, update ROB entry and clear RS
        if (rs.isWriting()) {
            rs.fu->flush();
            updateROBEntry(rs.robTag, cdb.value, rob);
            rs.setNextStatus(ReservationStation::EMPTY);
            cout<<"DONE"<<endl;
            destRegs[rs.destination] = 0;
            rs.clear();
        }
    }
    for(auto &rs: rsList){
        if(!rs.isReady() && rs.issued()) {
            cout<<"Not Ready"<<endl;
            rs.setNextStatus(ReservationStation::ISSUED);
            rs.Qj = destRegs[rs.reg1];
            rs.Qk = destRegs[rs.reg2];
        }
    }
}

void TomasuloSimulator::commit() {
    if (!rob.empty() && rob.front().ready) {    // If ROB is not empty and the front entry is ready
        ReorderBuffer entry = rob.front();
        rob.pop();
        registers[entry.dest] = entry.value;
        tags[entry.tag] = false;
        robRegTable[entry.dest] = 0;

        // Handle branch misprediction
        if (entry.type == "BEQ" && entry.value != entry.actualPC) {
            rollback(entry.value); // Rollback to the correct PC
        }
    }
}

void TomasuloSimulator::rollback(int correctPC) {
    // Flush the ROB
    while (!rob.empty()) {
        rob.pop();
    }

    // Clear reservation stations
    for (auto &rs : rsList) {
        rs.clear();
    }

    // Reset functional units
    for (auto &fu : fuList) {
        fu.flush();
    }

    // Reset the PC and other state variables
    PC = correctPC;
    totalCycles = 0;    // FIXME: Adjust the cycle count
    fuResult = 0;
    fill(tags.begin(), tags.end(), false);
}

void TomasuloSimulator::printState() {
    cout << "Cycle: " << totalCycles << endl;
    cout << "Registers: ";
    for (int i = 0; i < registers.size(); i++) {
        cout << "R" << i << ": " << registers[i] << " ";
    }
    cout << endl;

    cout << "ROB: ";
    queue<ReorderBuffer> temp = rob;
    while (!temp.empty()) {
        ReorderBuffer entry = temp.front();
        temp.pop();
        cout << "ROB" << entry.tag << ": " << entry.value << " ";
    }
    cout << endl;

    cout << "RS: ";
    for (auto &rs : rsList) {
        cout << rs.name << ": ";
        if (rs.isBusy()) {
            cout << "Busy ";
        } else {
            cout << "Free ";
        }
    }
    cout << endl;

    cout << "CDB: ";
    if (cdb.isBusy()) {
        cout << "Busy ";
    } else {
        cout << "Free ";
    }
    cout << endl;
    cout << endl;
}

void TomasuloSimulator::advanceCycle() {
    for(auto &rs: rsList) {
        rs.applyNextStatus();
    }
}

int TomasuloSimulator::getTag() {
    for(int i=1; i<+tags.size(); i++) {
        if(!tags[i]) {
            tags[i] = true;
            return i;
        }
    }
    return -1;
}



void TomasuloSimulator::updateROBEntry(int tag, int16_t value, queue<ReorderBuffer> &rob) {
    queue<ReorderBuffer> temp;
    while(!rob.empty()) {
        ReorderBuffer entry = rob.front();
        rob.pop();
        if(entry.tag == tag) {
            entry.setReady(value);
        }
        temp.push(entry);
    }
    rob = temp;

//    printROB(rob.front());



}

bool TomasuloSimulator::isRSListEmpty() {
    for(auto &rs: rsList) {
        if(!(rs.isEmpty())) {
            return false;
        }
    }
    return true;
}

bool TomasuloSimulator::isFUListEmpty() {
    for(auto &fu: fuList) {

        if(!(fu.isEmpty())) {
            return false;
        }
    }
    return true;
}



