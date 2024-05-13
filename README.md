# RISC-V 32i System Emulator Written in C

This program was developed for my 2024 Capstone project. The goal behind the development of a CPU emulator was to learn more about computer architecture methodologies including instruction encoding, decoding, execution, and memory management. RISC-V was chosen as the architecture of study due to its open-source design and rise in popularity in modern chip design. 

The scope of this project focuses on the implementation of all RV32i instructions. 

## Credited References and Sources
* The foundation of my program came from a developer project repo of a 64i implementation (along with many additional extensions). My project modified this code to conform to the [RV32I Base Integer Instruction Set,
Version 2.0 documentation](https://riscv.org/wp-content/uploads/2017/05/riscv-spec-v2.2.pdf). 

[64i Emulator in C](https://github.com/fmash16/riscv_emulator)

[Tutorial Page Written By the Author](https://fmash16.github.io/content/posts/riscv-emulator-in-c.html)

## Tools and Research Links
* Additional tools and references used to understand and develop a RISC-V system.

[RISC-V Interpreter](https://www.cs.cornell.edu/courses/cs3410/2019sp/riscv/interpreter/)

[Reference Card For 32i Instructions](https://www.cs.sfu.ca/~ashriram/Courses/CS295/assets/notebooks/RISCV/RISCV_CARD.pdf)

[RISC-V 32i Instruction Breakdown](https://msyksphinz-self.github.io/riscv-isadoc/html/rvi.html#slti)

[Series of Lectures Breaking Down the ISA](https://www.youtube.com/playlist?list=PL3by7evD3F53Dz2RiB47Ztp9l_piGVuus)

[Great Slideshow From Berkeley About the Different Instruction Formats](https://inst.eecs.berkeley.edu/~cs61c/resources/su18_lec/Lecture7.pdf)

[RISC-V Toolchain GCC Options (-mabi, -march)](https://gcc.gnu.org/onlinedocs/gcc/RISC-V-Options.html)

[Link Options (-Wl & -nostdlib)](https://gcc.gnu.org/onlinedocs/gcc-4.4.2/gcc/Link-Options.html)

## Forums and Stack Overflow Pages Scrounged 
* Extra links to forums that helped me better understand how to use the RISC-V toolchain, write assembly, and configure my tools.  

[Using LUI and ADDI to Load 32-bit Constants](https://stackoverflow.com/questions/50742420/risc-v-build-32-bit-constants-with-lui-and-addi)

[Demonstration of Using objdump tool and more](https://www.reddit.com/r/RISCV/comments/l91qzu/toolchain_compiler_question/)

[A Manual For RISC-V ASM Programming](https://shakti.org.in/docs/risc-v-asm-manual.pdf)

[Another Look At Objdump](https://forums.sifive.com/t/gcc-assembling-problem/1271/2)

[Stopping Toolchain From Building Compressed (16-bit) Instructions](https://stackoverflow.com/questions/70017352/how-can-i-disable-compress-when-using-riscv32-toolchain)

[GitHub Issue Discussing New Standards 32i ISA](https://github.com/riscv-collab/riscv-gnu-toolchain/issues/1052)

## Build and Run [Emulator]

To build and run the emulator, clone the repo and do the following:

```bash
make
./main <binary.bin> [-d]
```

```-d``` &emsp; Dump the contents of all 32 32-bit registers after each executed instruction.

Single instruction ```binary.bin``` files live in ```./isa_tests/single/bin/```

Cross compiled C Program binaries live in ```./isa_tests/cprog/bin/```

## Run [Python Scripts]

To run the Python scripts that build the binaries, object files, and assembly (only for c programs), you'll need access to the [riscv-gnu-toolchain](https://github.com/riscv-collab/riscv-gnu-toolchain) cross-compiler.

If you choose to compile the tools yourself, follow the toolchain's instructions and run the following:

```bash
./configure --prefix=/opt/riscv --with-arch=rv32g --with-abi=ilp32d
make
```

Total build time took me around 2 hours using an [OrbStack](https://orbstack.dev/) Ubuntu VM. 

### Prebuilt Tools

You can download a prebuilt 32i version of the toolchain [here](https://github.com/stnolting/riscv-gcc-prebuilt/releases/tag/rv32i-131023). Be sure to untar it in /opt/riscv/ and run ```export PATH=$PATH:/opt/riscv/bin/``` to add the tools to your path. 

- However, this set of tools cannot produce working binaries from C programs due to how newlib functions. If you wish to compile them anyway, be sure to remove the -nostdlib option in the build_cprog.py script. 

## Testing

Testing was primarily done through self-verification using a [RISC-V Interpreter](https://www.cs.cornell.edu/courses/cs3410/2019sp/riscv/interpreter/) developed by Cornell University. 

I originally wanted to use the [riscv-tests](https://github.com/riscv-software-src/riscv-tests) repository but faced issues with the configuration of the toolchain. For future developments, I'll adjust my program and tools to take advantage of the test suite. 

## Future Developments

Future developments of this project will seek to implement the [Zicsr Standard Extension](https://riscv.org/wp-content/uploads/2019/06/riscv-spec.pdf#chapter.9) so that I can use the riscv-tests repo to perform more official testing and, additionally, add support to all of the classical extensions: [M](https://riscv.org/wp-content/uploads/2019/06/riscv-spec.pdf#chapter.7), [A](https://riscv.org/wp-content/uploads/2019/06/riscv-spec.pdf#chapter.8), [F](https://riscv.org/wp-content/uploads/2019/06/riscv-spec.pdf#chapter.11), [D](https://riscv.org/wp-content/uploads/2019/06/riscv-spec.pdf#chapter.12), and [C](https://riscv.org/wp-content/uploads/2019/06/riscv-spec.pdf#chapter.16).
