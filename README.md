# RV64I Core Design Using High-Level Synthesis (HLS)

## Overview

This project presents the design and implementation of a RISC-V RV64I Core using High-Level Synthesis (HLS).

## Usage Instructions

### CLI Usage
The project can be run directly on a terminal to verify using C simulation the correctness of the core. To do so:

**1. Build the Project**

To compile the simulator and assembler:
```bash
git clone https://github.com/Red-RobinHood/RISC-V-Core.git
cd RISC-V-Core
make all
```


**2. Run the Simulation**

Run the testbench to simulate core execution:

```bash
./sim_tb
```

**3. Cleaning Up**

Remove all compiled files and output logs:
```bash
make clean
```

### HLS Usage

To run the actual HLS synthesis we are using **Vitis HLS**. To run it copy the `sim.hpp` and `sim.cpp` files into the `Source` folder and the rest of the files int the `Test Bench` folder and run the software like routine.

## ISA Support

The core supports the following RV64I (64-bit Integer Instruction Set):

•	Arithmetic Operations: ADD, SUB (and their immediate counterparts)

•	Logical Operations: AND, OR, XOR, SLL, SRL, SRA (and their immediate counterparts)

•	Branching: BEQ, BNE, BLT, BLTU, BGE, BGEU, JAL, JALR

•	Load/Store: LD, LW, LH, LB, LWU, LHU, LBU, SD, SH, SW, SB

•	Upper Immediate: LUI

## Limitations

The core currently does not support the .data section due to limitations in the assembler, which lacks the capability to handle:

•	Static/global variable initialization.

•	Memory allocation using directives like .word, .byte, .space, and .align.

As a result, data must be manually initialized at runtime using immediate values or through memory-mapped I/O.

The assembler is designed to handle only a subset of standard RISC-V assembly:

•	Supports basic instruction translation without pseudo-instructions.

•	Does not support assembler directives like .globl, .section, .bss, etc.

However these can easily be fixed.

## Acknowledgments

This project was developed under the guidance of Dr. Rajesh Kedia, with a focus on RISC-V architecture and HLS-based design methodologies. Thanks to [@lakshsidhu04](https://github.com/lakshsidhu04) for the assembler.
