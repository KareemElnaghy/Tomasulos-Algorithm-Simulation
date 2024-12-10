# Tomasulos-Algorithm-Simulation

Student Names:
- Jana Elfeky
- Kareem Elnaghy

Student IDs:
- 900215017
- 900223587

Course: CSCE3301 - Computer Architecture
Instructor: Dr. Cherif Salama

Release Notes:

1. **Issues:**
   - Some edge cases with the instructions tend to cause issues and the CALL instruction does not always function as expected.
   - Error handling could be improved.

2. **Assumptions:**
   - Assumes input programs follow a specific format with operations and operands in a consistent way.
   - Instruction latencies and configuration values should be entered correctly for simulation accuracy.

3. **What Works:**
   - The simulation successfully parses and executes a range of RiSC-16 instructions (ADD, ADDI, MUL, NAND, LOAD, STORE, BEQ, CALL, RET).
   - Tomasulo's algorithm works for resolving data hazards using reservation stations and a reorder buffer.
   - The parser supports label resolution and offset calculations for jumps and function calls.
   - The simulator supports configurable hardware setup, including the number of reservation stations and latencies for different instruction types.

4. **What Does Not Work:**
   - Speculative execution for branches is not fully optimized and may introduce performance issues for some test cases.
   - Inconsistent functionality with CALL and BEQ.

5. **Known Limitations:**
   - Currently only a subset of RiSC-16 instructions are supported. Some complex instructions and optimizations ave not been implemented.
   - The program assumes memory addresses are handled sequentially and may not support dynamic memory allocation scenarios.
   

Instructions for Running the Simulation:

1. Prepare Input File:
   Create a text file with a list of instructions in the correct format. Place the file in the same directory as the simulator or specify its path during execution.

2. Set Up the Configuration:
   The program will prompt you for the number of reservation stations and latencies for each instruction type. Enter appropriate values for simulation.

3. Start the Simulation:
   After configuration, the simulation begins and processes the instructions based on Tomasulo’s algorithm. The results will be shown with detailed logs.

4. View Output:
   After the simulation completes, the output will display the results, including cycle counts, throughput, and other key metrics.
