#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include "bus.h"

typedef struct CPU {
    uint32_t regs[32];          // 32 32-bit registers (x0-x31)
    uint32_t pc;                // 32-bit program counter
    struct BUS bus;             // CPU connected to BUS
} CPU;

/*************
 * cpu_init
 *   Zero initializes all 32 registers and sets register x02(sp) to DRAM_BASE+DRAM_SIZE.
 *   Sets pc to the start of mem which contains the first instruction (pc=DRAM_BASE)
 *************/
void cpu_init(struct CPU *cpu);

/*************
 * cpu_fetch
 *   Fetches instruction data from the dram at the pc address
 *   Data loaded onto the bus via dram_load()
 *************/
uint32_t cpu_fetch(struct CPU *cpu);

/*************
 * cpu_execute
 *   Identifies and "Executes" an instruction by parsing the passed in 4 byte value (instr)
 *   into it's respective parts. Depending on the identified opcode taken from bits 6..0, 
 *   a selected exec function is chosen and further, more specific, parsing of the instruction
 *   is performed. 
 *************/
int cpu_execute(struct CPU *cpu, uint32_t inst);

/*************
 * dump_registers
 *   Prints all 32 32-bit registers (x0-x31) and their respective values
 *   to stdout after every executed instruction. Enabled by passing the option
 *   -d through the cmdline.  
 *************/
void dump_registers(struct CPU *cpu);
#endif
