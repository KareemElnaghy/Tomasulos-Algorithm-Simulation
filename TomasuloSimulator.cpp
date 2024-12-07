//
// Created by Asus on 12/4/2024.
//
#include "TomasuloSimulator.h"
#include <iomanip>
ReservationStation *rsPointer;
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

#include <sstream>

string getInstruction(const Instruction& inst)
{
    stringstream ss;

    if (inst.op == "ADD" || inst.op == "MUL" || inst.op == "NAND")
    {
        ss << inst.op << " R" << inst.rd << ", R" << inst.rs1 << ", R" << inst.rs2;
    }
    else if (inst.op == "ADDI")
    {
        ss << inst.op << " R" << inst.rd << ", R" << inst.rs1 << ", " << inst.offset;
    }
    else if (inst.op == "BEQ")
    {
        ss << inst.op << " R" << inst.rs1 << ", R" << inst.rs2 << ", " << inst.offset;
    }
    else if (inst.op == "LOAD")
    {
        ss << inst.op << " R" << inst.rd << ", " << inst.offset << "(R" << inst.rs1 << ")";
    }
    else if (inst.op == "STORE")
    {
        ss << inst.op << " R" << inst.rs1 << ", " << inst.offset << "(R" << inst.rs2 << ")";
    }
    else if (inst.op == "CALL")
    {
        ss << inst.op << " " << inst.label;
    }
    else if (inst.op == "RET")
    {
        ss << inst.op;
    }
    else
    {
        ss << "Unknown instruction: " << inst.op;
    }

    return ss.str();
}


void printCycleMap(Instruction inst)
{
    cout << "ISSUED: " << inst.getIssueCycle()<< endl;
    cout << "STARTED_EXECUTION: " << inst.getStartExecCycle()<< endl;
    cout << "FINISHED_EXECUTION: " << inst.getFinishExecCycle() << endl;
    cout << "WRITTEN: " << inst.getWriteCycle() << endl;
    cout << "COMMITTED: " << inst.getCommitCycle() << endl;
}


void printTable(vector<Instruction>& instructions) {
    // Adjusted column widths for better alignment
    const int instrWidth = 20;
    const int issuedWidth = 12;
    const int execStartWidth = 15;
    const int execFinishWidth = 15;
    const int writtenWidth = 12;
    const int committedWidth = 12;

    // Print header
    cout << setw(instrWidth) << "Instruction"
         << setw(issuedWidth) << "Issued"
         << setw(execStartWidth) << "Started Exec"
         << setw(execFinishWidth) << "Finished Exec"
         << setw(writtenWidth) << "Written"
         << setw(committedWidth) << "Committed" << endl;

    // Print a separator line with dashes
    cout << string(instrWidth + issuedWidth + execStartWidth + execFinishWidth + writtenWidth + committedWidth, '-') << endl;

    // Print details for each instruction
    for (auto& inst : instructions) {
        cout << setw(instrWidth) << getInstruction(inst) // Updated width
             << setw(issuedWidth) << inst.getIssueCycle()
             << setw(execStartWidth) << inst.getStartExecCycle()
             << setw(execFinishWidth) << inst.getFinishExecCycle()
             << setw(writtenWidth) << inst.getWriteCycle()
             << setw(committedWidth) << inst.getCommitCycle() << endl;
    }

    // Print a separator line at the bottom
    cout << string(instrWidth + issuedWidth + execStartWidth + execFinishWidth + writtenWidth + committedWidth, '-') << endl;
}



TomasuloSimulator::TomasuloSimulator(vector<Instruction> instructions, vector<int16_t> memory, int startingPC, int robCapacity, unordered_map<string, int> stationCount,
                                     unordered_map<string, int> latency)
: instructions(instructions), memory(memory), PC(startingPC), robCapacity(robCapacity)
{
    totalCycles = 0;
    totalBranches = 0;
    totalBranchMispredictions = 0;
    registers.resize(8, 0);
    destRegs.resize(8, 0);
    fuResult = 0;
    tags.resize(8, false);

    for(auto &p: stationCount) {
        for(int i=0; i<p.second; i++) {
            rsList.emplace_back(p.first, to_string(i+1));
            FunctionalUnit fu(p.first, to_string(i+1), latency[p.first]);
            fuList.emplace_back(fu);
        }
    }

}

void TomasuloSimulator::simulate() {
    // TODO: Implement all the various counters
    int counter = 0;

    while (PC < instructions.size() || !rob.empty() || !isRSListEmpty() || !isFUListEmpty()) {
//        cout<<"====Cycle "<<totalCycles<<"======"<<endl;
//        cout<<"PC: "<<PC<<endl;
        advanceCycle(); // Advance the cycle
        issue();        // Issue the instruction
        execute();      // Execute the instruction
        write();        // Write the result to the CDB
        commit();       // Commit the result to the register
        counter++;
//        for(auto &rs: rsList) {
//            if(rs.getStatus() == "ISSUED")
//            {
//                instructions[rs.instPC].setIssueCycle(totalCycles);
//            }
//        }
        totalCycles++;  // Increment the cycle count

//        for(auto &rs: rsList) {
//            if(rs.name == "CALL/RET" || rs.name == "ADD/ADDI" || rs.name == "NAND" || rs.name == "BEQ")
//            printReservationStation(rs);
//        }
//
//        if(counter == 30)
//        {
//            break;
//        }

    }

    float branchMissPredictionRate = (totalBranches != 0)?((float)totalBranchMispredictions/totalBranches)*100:0;

    printTable(instructions);
    cout<<endl;
    cout<<"Performance Metrics"<<endl;
    cout<<"Total Cycles: "<<totalCycles-1<<endl;
    cout<<"IPC: "<<(float)totalInstructions/totalCycles-1<<endl;
    cout<<"Branches: "<<totalBranches<<endl;
    cout<<"Branch Mispredictions: "<<totalBranchMispredictions<<endl;
    cout<<"Branch Misprediction Percentage: "<<branchMissPredictionRate<<endl;

    for(int i = 0; i< 4; i++)
        cout<<"Memory["<<i<<"]: "<<memory[i]<<endl;
    printRegisters(registers);
    cout<<endl;
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
    if(inst.op == "BEQ")
    {
        totalBranches++;
    }

    // If ROB is full or no free RS, stall
    if (rob.size() >= robCapacity || !hasFreeRS(inst)) return;

    if(inst.rd == 0 && inst.op != "BEQ" && inst.op != "STORE" && inst.op != "RET")
    {
        totalInstructions++;
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
                rs.Qj = destRegs[inst.rs1];

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
            else if(rs.name == "CALL/RET")
            {
                // If CALL destination is R1 and sources are 0, if RET destination is R0 and Source is 1
                int dest = (rs.op == "CALL") ? 1 : 0; // if CALL we write to R1
                ReorderBuffer robEntry(tag, inst.op, dest, PC+1);  // Create a new ROB entry
                rob.push(robEntry); // Push the ROB entry to the ROB
                rs.robTag = tag;
                rs.busy = true;
                rs.op = inst.op;
                rs.A = (rs.op == "CALL")? inst.label : 0;
                destRegs[dest] = (rs.op == "CALL")? tag : destRegs[dest];
                rs.instPC = PC;
                rs.destination = dest;
                rs.reg1 = (rs.op == "RET")? 1 : 0;

                if(rs.op == "RET"){
                    rs.Qj = destRegs[inst.rs1];

                    if (rs.Qj == cdb.tag) {
                        rs.Vj = cdb.value;
                        rs.Qj = 0;
                    } else if (rs.Qj == 0) {
                        rs.Vj = registers[inst.rs1];
                    } else {
                        rs.Vj = 0;
                    }
                }
                rs.reg2 = 0;
            }
            else {
                ReorderBuffer robEntry(tag, inst.op, inst.rd, PC + 1);
                rob.push(robEntry); // Push the ROB entry to the ROB
                rs.robTag = tag;
                destRegs[inst.rd] = tag;
                rs.busy = true;
                rs.op = inst.op;
                rs.destination = inst.rd;
                rs.A = inst.offset;
                rs.instPC = PC;
                rs.reg1 = inst.rs1;
                rs.reg2 = inst.rs2;

                // Check if rs1 is ready
                rs.Qj = destRegs[inst.rs1];

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

                if (rs.Qk == cdb.tag && rs.makeOperand2ReadyFlag) {   // If rs2 is not -1, set Vk and Qk
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
        if(!rs.isReady() && rs.issued()&& cdb.isBusy())
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
            if(nextInst.op == "ADDI" && nextInst.rs1 == 0 && nextInst.rd == 0 && nextInst.rs2 == 0 && rs.name == "ADD/ADDI")
            {
                rs.setNextStatus(ReservationStation::EMPTY);
                break;
            }
            if (!rs.isBusy() && (rs.name.find(nextInst.op) != string::npos) )
            {
                rs.setNextStatus(ReservationStation::ISSUED);
                break;
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
                if ((fu.name == rs.name) && (fu.unit == rs.unit) && !fu.isBusy()) {
                    fu.operation = rs.op;
                    fu.operand1 = rs.Vj;
                    fu.operand2 = rs.Vk;
                    fu.A = rs.A; // Label or Offset

                    FunctionalUnit *fuPointer = &fu;
                    rs.setFunctionalUnit(fuPointer);
                    fu.startExec();
                    break;
                }
            }
        }



        // If RS is executing and FU is not done

        if (rs.isExecuting() && rs.fu->getRemCycles() > 0) {
            if(rs.fu->remainingCycles == rs.fu->latency - 1)
            {
                instructions[rs.instPC].setStartExecCycle(totalCycles-1); // FIXME: set start exec cycle
            }
            rs.setNextStatus(ReservationStation::EXECUTING);
            rs.fu->execute();
        }

        if(rs.isExecuting() && rs.fu->remainingCycles == 0) {
            instructions[rs.instPC].setFinishExecCycle(totalCycles); // FIXME: set finish exec cycle
            if (rs.op == "STORE") {
                int result = rs.fu->getResult(rs.instPC, PC);
                updateROBEntry(rs.robTag, result, rob);
                rs.setNextStatus(ReservationStation::WRITING);
                continue;
            }

            if (rs.op == "BEQ") {
                int targetAddr = rs.fu->getResult(rs.instPC, PC);
                cdb.writeToCDB(targetAddr, rs.robTag);
                rs.setNextStatus(ReservationStation::WRITING);
                continue;
            }

            if (rs.op == "LOAD") {
                rs.setNextStatus(ReservationStation::WRITING);
                continue;
            }

            if (rs.op == "CALL") {
                int targetAddr = rs.fu->getResult(rs.instPC, PC);
                cdb.writeToCDB(targetAddr, rs.instPC + 1, rs.robTag);
                rs.setNextStatus(ReservationStation::WRITING);
                continue;
            }

            if (rs.op == "RET") {
                int targetAddr = rs.fu->getResult(rs.instPC, PC);
                cdb.writeToCDB(targetAddr, rs.robTag);
                rs.setNextStatus(ReservationStation::WRITING);
                continue;
            }

            if (!rs.readyToWrite) {
                if (!cdb.isBusy()) {
                    fuResult = rs.fu->getResult(rs.instPC, PC);
                    rs.setNextStatus(ReservationStation::WRITING);
                    cdb.writeToCDB(fuResult, rs.robTag);

                } else {
                    rs.readyToWrite = true;
                    rs.setNextStatus(ReservationStation::WRITING);
                }
            }
        }

    }
}

void TomasuloSimulator::write() {
    for (auto &rs : rsList) {

        if(rs.isWriting()){// If RS is writing, update ROB entry and clear RS
            if (rs.op == "STORE") {
                rs.fu->flush();
                rs.setNextStatus(ReservationStation::EMPTY);
                rs.clear();
                cdb.currBusy = false;
                instructions[rs.instPC].setWriteCycle(totalCycles); // FIXME: set write cycle
                continue;
            } else if (rs.op == "LOAD" && rs.remCyclesLoad > 0) {
                rs.remCyclesLoad--;
                rs.setNextStatus(ReservationStation::WRITING);
                cdb.currBusy = false;
                continue;
            } else if (rs.op == "LOAD" && rs.remCyclesLoad == 0) {
                int address = rs.fu->getResult(rs.instPC, PC);
                cdb.writeToCDB(memory[address], rs.robTag);
                updateROBEntry(rs.robTag, memory[address], rob);
                for (auto &curRS: rsList) {
                    if (curRS.Qj == rs.robTag) {
                        curRS.makeOperand1ReadyFlag = true;
                        curRS.Vj = cdb.value;
                        curRS.Qj = 0;
                    }
                    if (curRS.Qk == rs.robTag) {
                        curRS.makeOperand2ReadyFlag = true;
                        curRS.Vk = cdb.value;
                        curRS.Qk = 0;
                    }
                    if (curRS.isBusy() && curRS.isReady() && curRS.issued()) {
                        curRS.setNextStatus(ReservationStation::EXECUTING);

                        for (auto &fu: fuList) {   // Find a free FU and start execution
                            if ((fu.name == curRS.name) && (fu.unit == curRS.unit) && !fu.isBusy()) {
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
                cdb.currBusy = false;
                instructions[rs.instPC].setWriteCycle(totalCycles); // FIXME: set write cycle
                continue;
            } else if (rs.op == "BEQ") {
                rs.fu->flush();
                updateROBEntry(rs.robTag, cdb.value, rob);
                rs.setNextStatus(ReservationStation::EMPTY);
                rs.clear();
                cdb.currBusy = false;
                instructions[rs.instPC].setWriteCycle(totalCycles); // FIXME: set write cycle
                continue;
            } else if (rs.op == "CALL") {
                updateROBEntry(rs.robTag, cdb.value, cdb.extraValue, rob);
                rs.setNextStatus(ReservationStation::EMPTY);
                rs.fu->flush();
                rs.clear();
                cdb.currBusy = false;
                instructions[rs.instPC].setWriteCycle(totalCycles); // FIXME: set write cycle
                continue;
            } else if (rs.op == "RET") {
                updateROBEntry(rs.robTag, cdb.value, rob);
                rs.setNextStatus(ReservationStation::EMPTY);
                rs.fu->flush();
                rs.clear();
                cdb.currBusy = false;
                instructions[rs.instPC].setWriteCycle(totalCycles); // FIXME: set write cycle
                continue;
            } else {
                rs.fu->flush();
                updateROBEntry(rs.robTag, cdb.value, rob);
                for (auto &curRS: rsList) {
                    if (curRS.Qj == rs.robTag) {
                        curRS.makeOperand1ReadyFlag = true;
                        curRS.Vj = cdb.value;
                        curRS.Qj = 0;
                    }
                    if (curRS.Qk == rs.robTag) {
                        curRS.makeOperand2ReadyFlag = true;
                        curRS.Vk = cdb.value;
                        curRS.Qk = 0;
                    }
                    if (curRS.isBusy() && curRS.isReady() && curRS.issued()) {
                        curRS.setNextStatus(ReservationStation::EXECUTING);

                        for (auto &fu: fuList) {   // Find a free FU and start execution
                            if ((fu.name == curRS.name) && (fu.unit == curRS.unit) && !fu.isBusy()) {
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
                cdb.currBusy = false;
                instructions[rs.instPC].setWriteCycle(totalCycles); // FIXME: set write cycle
                continue;
            }

        }

    }
    for(auto &rs: rsList) {
        if(rs.isReadyToWrite() && !cdb.isBusy())
        {
            rs.readyToWrite = false;
            fuResult = rs.fu->getResult(rs.instPC, PC);
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
        instructions[entry.actualPC-1].setCommitCycle(totalCycles); // FIXME: set commit cycle
        totalInstructions++;
        if(entry.type == "STORE")
        {
            memory[entry.value] = registers[entry.dest];
        }
        else{
            registers[entry.dest] = (entry.dest != 0) ? entry.value : 0;
        }

        tags[entry.tag] = false;

        // Handle branch misprediction
        if ((entry.type == "BEQ" && entry.value != entry.actualPC) | entry.type == "CALL" | entry.type == "RET") {
            if(entry.type == "BEQ")
            {
                totalBranchMispredictions++;
            }

            rollback(entry.value); // Rollback to the correct PC
            registers[1] = (entry.type == "CALL") ? entry.extraValue : registers[1];
        }
        destRegs[entry.dest] = 0;

    }
    if(!rob.empty() && rob.front().willBeReady)
    {
        makeReady(rob.front().tag, rob);
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
            entry.setReady(value);  // mark it as will be ready
        }
        temp.push(entry);
    }
    rob = temp;

}

void TomasuloSimulator::updateROBEntry(int tag, int16_t value, int16_t extraValue, queue<ReorderBuffer> &rob) {
    queue<ReorderBuffer> temp;
    while(!rob.empty()) {
        ReorderBuffer entry = rob.front();
        rob.pop();
        if(entry.tag == tag) {
            entry.value = value;
            entry.extraValue = extraValue;
            entry.ready = true;
        }
        temp.push(entry);
    }
    rob = temp;
}


void TomasuloSimulator::makeReady(int tag, queue<ReorderBuffer> &rob) {
    queue<ReorderBuffer> temp;
    while(!rob.empty()) {
        ReorderBuffer entry = rob.front();
        rob.pop();
        if(entry.tag == tag) {
            entry.ready = true;
            entry.willBeReady = false;
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

