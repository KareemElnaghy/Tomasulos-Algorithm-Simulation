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

void printInstruction(const Instruction& inst)
{
    if (inst.op == "ADD" || inst.op == "MUL" || inst.op == "NAND")
    {
        cout << inst.op << " R" << inst.rd << ", R" << inst.rs1 << ", R" << inst.rs2 << endl;
    }
    else if (inst.op == "ADDI")
    {
        cout << inst.op << " R" << inst.rd << ", R" << inst.rs1 << ", " << inst.offset << endl;
    }
    else if (inst.op == "BEQ")
    {
        cout << inst.op << " R" << inst.rs1 << ", R" << inst.rs2 << ", " << inst.offset << endl;
    }
    else if (inst.op == "LOAD")
    {
        cout << inst.op << " R" << inst.rd << ", " << inst.offset << "(R" << inst.rs1 << ")" << endl;
    }
    else if (inst.op == "STORE")
    {
        cout << inst.op << " R" << inst.rs1 << ", " << inst.offset << "(R" << inst.rs2 << ")" << endl;
    }
    else if (inst.op == "CALL")
    {
        cout << inst.op << " " << inst.label << endl;
    }
    else if (inst.op == "RET")
    {
        cout << inst.op << endl;
    }
    else
    {
        cout << "Unknown instruction: " << inst.op << endl;
    }
}


TomasuloSimulator::TomasuloSimulator(vector<Instruction> instructions, vector<int16_t> memory, int startingPC, int robCapacity, unordered_map<string, int> stationCount)
: instructions(instructions), memory(memory), PC(startingPC), robCapacity(robCapacity)
{
    totalCycles = 0;
    registers.resize(8, 0);
    destRegs.resize(8, 0);
    robRegTable.resize(8, 0);
    fuResult = 0;
    tags.resize(8, false);

    for(auto &p: stationCount) {
        for(int i=0; i<p.second; i++) {
            rsList.emplace_back(p.first, to_string(i+1));
            FunctionalUnit fu(p.first, to_string(i+1));
            fuList.emplace_back(p.first, to_string(i+1));
        }
    }

    for(auto &inst: instructions) {
        printInstruction(inst);
    }

}

void TomasuloSimulator::simulate() {
    // TODO: Implement all the various counters
    // FIXME: Reservation Table Info displays a cycle early (I think)
    // FIXME: Execution should start when the operands are ready AND prev instructions are done broadcasting to CDB
    int counter = 0;
    while (PC < instructions.size() || !rob.empty() || !isRSListEmpty() || !isFUListEmpty()) {
        cout<<"=====================================CYCLE "<<totalCycles<<"========================================"<<endl;
        cout<<"Current PC: "<<PC<<endl;
        advanceCycle(); // Advance the cycle
        issue();        // Issue the instruction
        execute();      // Execute the instruction
        write();        // Write the result to the CDB
        commit();       // Commit the result to the register
        // printState(); // Print the state of the simulator
        totalCycles++;  // Increment the cycle count
        counter++;      // for debugging

        cout<<"RESERVATION STATIONS"<<endl;
        for (auto &rs : rsList) {
            if (rs.name+rs.unit == "STORE1" || rs.name+rs.unit == "BEQ1" || rs.name+rs.unit == "ADD/ADDI1" || rs.name+rs.unit == "NAND1" || rs.name+rs.unit == "LOAD1" || rs.name+rs.unit == "MUL1"){
                printReservationStation(rs);
            }
        }
        printROB(rob.front());
        printRegisters(registers);

    }

//    printRegisters(registers);
    cout<<"Memory "<<memory[0]<<endl;
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
    printInstruction(inst);

    // If ROB is full or no free RS, stall
    if (rob.size() >= robCapacity || !hasFreeRS(inst)) return;
    if(inst.rd == 0 && inst.op != "BEQ" && inst.op != "STORE")
    {
        PC++;
        return;
    }



    // Find a free RS, issue the instruction, and increment PC
    for (auto &rs : rsList) {
        if (rs.issued() && !rs.isBusy()) { // If RS is not busy and operation matches

            int tag = getTag();
            if(rs.name == "STORE" || rs.name == "BEQ")
            {
                int dest = (rs.name == "STORE") ? inst.rs1 : inst.rd;
                ReorderBuffer robEntry(tag, inst.op, dest, PC+1);  // Create a new ROB entry
                rob.push(robEntry); // Push the ROB entry to the ROB
                rs.robTag = tag;
                rs.busy = true;
                rs.op = inst.op;
                rs.A = inst.offset;
                rs.instPC = PC;
                rs.reg1 = inst.rs1;
                rs.reg2 = inst.rs2;

                // Check if rs1 is ready
                rs.Qj = destRegs[inst.rs1]; // FIXME: Check rs1/rs2 if its -1 first

                // Check if rs2 is ready
                rs.Qk = destRegs[inst.rs2];

                // Set Vj and Vk
                if (rs.Qj == cdb.tag && rs.makeOperand1ReadyFlag) {   // If rs1 is not -1, set Vj and Qj
                    rs.Vj = cdb.value;
                    rs.Qj = 0;
                }
                else if(rs.Qj == 0)
                {
                    rs.Vj = registers[inst.rs1];
                }
                else {
                    rs.Vj = 0;
                }

                if (inst.rs2 != -1 && rs.Qk == cdb.tag) {   // If rs2 is not -1, set Vk and Qk
                    rs.Vk = cdb.value;
                    rs.Qk = 0;
                }
                else if(rs.Qk == 0)
                {
                    rs.Vk = registers[inst.rs2];
                }
                else {
                    rs.Vk = 0;
                }
            }
            else {
                ReorderBuffer robEntry(tag, inst.op, inst.rd, PC + 1);
                rob.push(robEntry); // Push the ROB entry to the ROB
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
                if (rs.Qj == cdb.tag) {   // If rs1 is not -1, set Vj and Qj
                    rs.Vj = cdb.value; // FIXME: retrieve value from CDB if Qj is not 0
                    rs.Qj = 0;
                }
                else if(rs.Qj == 0)
                {
                    rs.Vj = registers[inst.rs1];
                }
                else {
                    rs.Vj = 0;
                }

                if (inst.rs2 != -1 && rs.Qk == cdb.tag) {   // If rs2 is not -1, set Vk and Qk
                    rs.Vk = cdb.value;
                    rs.Qk = 0;
                }
                else if(rs.Qk == 0)
                {
                    rs.Vk = registers[inst.rs2];
                }
                else {
                    rs.Vk = 0;
                }
            }  // Create a new ROB entry


            PC++;   // Increment PC
            break;
        }
    }

    for(auto &rs: rsList)
    {
        if(!rs.isReady() && rs.issued())
        {
            if(rs.makeOperand1ReadyFlag)
            {
                rs.Vj = cdb.value;
                rs.Qj = 0;
            }
            if(rs.makeOperand2ReadyFlag)
            {
                rs.Vk = cdb.value;
                rs.Qk = 0;
            }
            rs.setNextStatus(ReservationStation::ISSUED);
        }
    }

    for(auto &rs: rsList) {
        if(PC < instructions.size())
        {
            Instruction nextInst = instructions[PC];
            if (!rs.isBusy() && (rs.name.find(nextInst.op) != string::npos) )
            {
                rs.setNextStatus(ReservationStation::ISSUED);
            }

            if(nextInst.op == "ADDI" && nextInst.rs1 == 0 && nextInst.rd == 0 && nextInst.rs2 == 0 && rs.name == "ADD/ADDI")
            {
                rs.setNextStatus(ReservationStation::EMPTY);
            }

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

        if(rs.isExecuting() && rs.fu->getRemCycles() == 0 && rs.op == "STORE")
        {
            int result = rs.fu->getResult(rs.instPC, PC);
            updateROBEntry(rs.robTag, result, rob);
            rs.setNextStatus(ReservationStation::WRITING);
            continue;
        }

        if(rs.isExecuting() && rs.fu->getRemCycles() == 0 && rs.op == "BEQ")
        {
            int targetAddr = rs.fu->getResult(rs.instPC, PC);
            cdb.writeToCDB(targetAddr, rs.robTag);
            rs.setNextStatus(ReservationStation::WRITING);
            continue;
        }

        if(rs.isExecuting() && rs.fu->getRemCycles() == 0 && rs.op == "LOAD")
        {
            rs.setNextStatus(ReservationStation::WRITING);
            continue;
        }

        if (rs.isExecuting() && rs.fu->getRemCycles() == 0) {
            if(!cdb.isBusy())
            {
                fuResult = rs.fu->getResult(rs.instPC, PC);
                rs.setNextStatus(ReservationStation::WRITING);
                cdb.writeToCDB(fuResult, rs.robTag);
            }
            else
            {
                rs.readyToWrite = true;
            }
        }

    }
}

void TomasuloSimulator::write() {
    bool writing = false;
    for (auto &rs : rsList) {

        // If RS is writing, update ROB entry and clear RS
        if(rs.op == "STORE" && rs.isWriting())
        {
            writing = true;
            rs.fu->flush();
            rs.setNextStatus(ReservationStation::EMPTY);
            rs.clear();

            continue;
        }
        else if(rs.op == "LOAD" && rs.isWriting() && rs.remCyclesLoad > 0)
        {
            rs.remCyclesLoad--;
            rs.setNextStatus(ReservationStation::WRITING);
            continue;
        }
        else if(rs.op == "LOAD" && rs.isWriting() && rs.remCyclesLoad == 0)
        {
            writing = true;
            int address = rs.fu->getResult(rs.instPC, PC);
            cdb.writeToCDB(memory[address], rs.robTag);
            updateROBEntry(rs.robTag, memory[address], rob);
            for(auto &curRS: rsList) {
                if(curRS.Qj == rs.robTag) {
                    curRS.makeOperand1ReadyFlag = true;
                    curRS.Vj = cdb.value;
                    curRS.Qj = 0;
                }
                if(curRS.Qk == rs.robTag) {
                    curRS.makeOperand2ReadyFlag = true;
                    curRS.Vk = cdb.value;
                    curRS.Qk = 0;
                }
                if (curRS.isBusy() && curRS.isReady() && curRS.issued()) {
                    curRS.setNextStatus(ReservationStation::EXECUTING);

                    for (auto &fu : fuList) {   // Find a free FU and start execution
                        if (fu.name.find(curRS.op) != string::npos && !fu.isBusy()) {
                            fu.operation = curRS.op;

                            fu.operand1 = curRS.Vj;
                            fu.operand2 = curRS.Vk;
                            fu.A = curRS.A;

                            FunctionalUnit *fuPointer = &fu;
                            curRS.setFunctionalUnit(fuPointer);

                            fu.startExec();

                            break;
                        }
                    }
                }
            }
            rs.fu->flush();
            rs.setNextStatus(ReservationStation::EMPTY);
            rs.clear();
            continue;
        }
        else if(rs.op == "BEQ" && rs.isWriting())
        {
            writing = true;
            rs.fu->flush();
            updateROBEntry(rs.robTag, cdb.value, rob);
            rs.setNextStatus(ReservationStation::EMPTY);
            rs.clear();
            continue;
        }
        else if (rs.isWriting()) {
            writing = true;
            rs.fu->flush();
            updateROBEntry(rs.robTag, cdb.value, rob);
            for(auto &curRS: rsList) {
                if(curRS.Qj == rs.robTag) {
                    curRS.makeOperand1ReadyFlag = true;
                    curRS.Vj = cdb.value;
                    curRS.Qj = 0;
                }
                if(curRS.Qk == rs.robTag) {
                    curRS.makeOperand2ReadyFlag = true;
                    curRS.Vk = cdb.value;
                    curRS.Qk = 0;
                }
                if (curRS.isBusy() && curRS.isReady() && curRS.issued()) {
                    curRS.setNextStatus(ReservationStation::EXECUTING);

                    for (auto &fu : fuList) {   // Find a free FU and start execution
                        if (fu.name.find(curRS.op) != string::npos && !fu.isBusy()) {
                            fu.operation = curRS.op;
                            fu.operand1 = curRS.Vj;
                            fu.operand2 = curRS.Vk;
                            fu.A = curRS.A;

                            FunctionalUnit *fuPointer = &fu;
                            curRS.setFunctionalUnit(fuPointer);
                            fu.startExec();

                            break;
                        }
                    }
                }
            }
            rs.setNextStatus(ReservationStation::EMPTY);
            rs.clear();
            continue;
        }


    }


    for(auto &rs: rsList) {
        if(rs.isReadyToWrite() && !cdb.isBusy())
        {
            printFU(*rs.fu);
            fuResult = rs.fu->getResult(rs.instPC, PC);
            rs.setNextStatus(ReservationStation::WRITING);
            cdb.writeToCDB(fuResult, rs.robTag);
        }
    }
    for(auto &rs: rsList){
        if(!rs.isReady() && rs.issued()) {
            rs.setNextStatus(ReservationStation::ISSUED);
        }
    }

}

void TomasuloSimulator::commit() {
    if (!rob.empty() && rob.front().ready) {    // If ROB is not empty and the front entry is ready
        ReorderBuffer entry = rob.front();

        if(entry.type == "STORE" && entry.remCycles > 0)
        {
            decrementRemCycle(entry.tag, rob);
            return;
        }

        rob.pop();
        if(entry.type == "STORE")
        {
            memory[entry.value] = registers[entry.dest];
        }
        else{
            registers[entry.dest] = (entry.dest != 0) ? entry.value : 0;
        }

        tags[entry.tag] = false;
        robRegTable[entry.dest] = 0;

        // Handle branch misprediction
        if (entry.type == "BEQ" && entry.value != entry.actualPC) {
            rollback(entry.value); // Rollback to the correct PC
        }
        destRegs[entry.dest] = 0;

    }

}

void TomasuloSimulator::rollback(int correctPC) {
    Instruction correctInst = instructions[correctPC];
    // Flush the ROB
    while (!rob.empty()) {
        rob.pop();
    }

    // Clear reservation stations
    for (auto &rs : rsList) {
        rs.clear();
        rs.status = ReservationStation::EMPTY;
        rs.nextStatus = ReservationStation::EMPTY;
    }

    // Reset functional units
    for (auto &fu : fuList) {
        fu.flush();
    }

    cdb.clear();

    for(auto &rs: rsList) {
        if (rs.name.find(correctInst.op) != string::npos )
        {
            rs.setNextStatus(ReservationStation::ISSUED);
            break;
        }
    }
    // Reset the PC and other state variables
    PC = correctPC;
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

}

void TomasuloSimulator::decrementRemCycle(int tag, queue<ReorderBuffer> &rob)
{
    queue<ReorderBuffer> temp;
    while(!rob.empty()) {
        ReorderBuffer entry = rob.front();
        rob.pop();
        if(entry.tag == tag) {
            entry.remCycles--;
        }
        temp.push(entry);
    }
    rob = temp;
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

void setROBFrontReady(queue<ReorderBuffer> &rob)
{
    if (!rob.empty()) {
        ReorderBuffer &entry = rob.front();
        entry.ready = true;
    }
}

