# RISC-V 32i System Emulator Written in C

This program was developed for my 2024 Capstone project. The goal behind the develeopment of a CPU emulator was aimed at learning more about computer architecture methedologies including: instruction encoding, decoding, execution and memory management. RISC-V was chosen as the architecture of study due to its open souce design and rise in popularity in modern chip design. 

The scope of this project focuses on the implementation of all RV32i instructions. 

## Build and Run [EMULATOR]

To build and run the emulator, clone the repo and do the following:

```bash
make
./main <binary.bin> [-d]
```

```-d``` &emsp;Dump the contents of all 32 32-bit registers after each exectued instruction.

Single instruction ```binary.bin``` files live in ```./isa_tests/single/bin/```

Cross compiled C Program binaries live in ```./isa_tests/cprog/bin/```

## Run [PYTHON SCRIPTS]

To run the python scripts that build the binaries, object files, and assembly (only for c programs), you'll need access to the [riscv-gnu-toolchain](https://github.com/riscv-collab/riscv-gnu-toolchain) cross-compiler.

If you choose to compile the tools yourself, follow the toolchain's instructions and run the following:

```bash
./configure --prefix=/opt/riscv --with-arch=rv32g --with-abi=ilp32d
make
```

Total build time took me around 2 hours uisng an [OrbStack](https://orbstack.dev/) Ubuntu VM. 

## Testing

Testing was primarily done through self-verification using a [RISC-V Interpreter](https://www.cs.cornell.edu/courses/cs3410/2019sp/riscv/interpreter/) developed by Cornell University. 

I originally wanted to use the [riscv-tests](https://github.com/riscv-software-src/riscv-tests) repository but faced issues with the configuration of the toolchain. For future developments, I'll adjust my program and tools to take advantage of the test suite. 
