/*
 * src/cpu.c
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "../includes/cpu.h"
#include "../includes/opcodes.h"

// printf colors
#define ANSI_YELLOW  "\x1b[33m"
#define ANSI_BLUE    "\x1b[31m"
#define ANSI_RESET   "\x1b[0m"

// print operation for DEBUG
void print_op(char* s) {
    printf("%s%s%s", ANSI_BLUE, s, ANSI_RESET);
}

void cpu_init(CPU *cpu) {
    cpu->regs[0] = 0x00;                    // register x0 hardwired to 0
    cpu->regs[2] = DRAM_BASE + DRAM_SIZE;   // Set stack pointer
    cpu->pc      = DRAM_BASE;               // Set program counter to the base address
}

// fetch 32-bit instruction from memory
uint32_t cpu_fetch(CPU *cpu) {
    uint32_t inst = bus_load(&(cpu->bus), cpu->pc, 32);
    return inst;
}

// load value from memory
uint32_t cpu_load(CPU* cpu, uint32_t addr, uint32_t size) {
    return bus_load(&(cpu->bus), addr, size);
}

// store value in memory
void cpu_store(CPU* cpu, uint32_t addr, uint32_t size, uint32_t value) {
    //printf("CPU STORE: addr=%#-13.2x, size=%d, value=%#-13.2x\n", addr, size, value);
    bus_store(&(cpu->bus), addr, size, value);
}

//=====================================================================================
// Instruction Decoder Functions
//=====================================================================================

//=============
// REGISTERS
//=============
// destination register stored in bits 11..7
uint32_t rd(uint32_t inst) {
    return (inst >> 7) & 0x1f;    // rd in bits 11..7
}
uint32_t rs1(uint32_t inst) {
    return (inst >> 15) & 0x1f;   // rs1 in bits 19..15
}
uint32_t rs2(uint32_t inst) {
    return (inst >> 20) & 0x1f;   // rs2 in bits 24..20
}

//================
// IMMEDIATES
//================
uint32_t imm_I(uint32_t inst) {
    // imm[11:0] = inst[31:20]
    return ((int32_t) (inst & 0xfff00000)) >> 20;
}
uint32_t imm_S(uint32_t inst) {
    // imm[11:5] = inst[31:25], imm[4:0] = inst[11:7]	
    return ((int32_t)(inst & 0xfe000000) >> 20)
        | ((inst >> 7) & 0x1f); 
}
uint32_t imm_B(uint32_t inst) {
    // imm[12|10:5|4:1|11] = inst[31|30:25|11:8|7]
    return ((int32_t)(inst & 0x80000000) >> 19)
        | ((inst & 0x80) << 4) // imm[11]
        | ((inst >> 20) & 0x7e0) // imm[10:5]
        | ((inst >> 7) & 0x1e); // imm[4:1]
}
uint32_t imm_U(uint32_t inst) {
    // imm[31:12] = inst[31:12]
    return (int32_t)(inst & 0xfffff000); // was 999
}
uint32_t imm_J(uint32_t inst) {
    // imm[20|10:1|11|19:12] = inst[31|30:21|20|19:12]
    return (uint32_t)((int32_t)(inst & 0x80000000) >> 11) 
        | (inst & 0xff000) // imm[19:12]
        | ((inst >> 9) & 0x800) // imm[11]
        | ((inst >> 20) & 0x7fe); // imm[10:1]
}

uint32_t shamt(uint32_t inst) {
    // shamt(shift amount) only required for immediate shift instructions
    // shamt[4:5] = imm[5:0]
    return (uint32_t) (imm_I(inst) & 0x1f);
}

//=====================================================================================
//   Instruction Execution Functions
//=====================================================================================

// load a 20-bit immediate value into the upper 20 bits of a 32-bit register
void exec_LUI(CPU* cpu, uint32_t inst) {
    uint32_t imm = imm_U(inst);
    cpu->regs[rd(inst)] = ((uint32_t)(int32_t)(imm));
    print_op("lui\n");
}

// add a 20-bit immediate value, zero-extended to 32 bits and shifted left by 12 bits, 
// to the address of the current instruction
void exec_AUIPC(CPU* cpu, uint32_t inst) {
    uint32_t imm = imm_U(inst);
    // pc = pc-DRAM_BASE; DRAM_BASE=0x80000000 
    cpu->regs[rd(inst)] = (((uint32_t)(int32_t) cpu->pc-DRAM_BASE) + ((uint32_t)(int32_t) imm));
    print_op("auipc\n");
}

// Perfoms an unconditional jump and stores the return addr in a register
// Return addr is of the instruction immediately after the jal
void exec_JAL(CPU* cpu, uint32_t inst) {
    uint32_t imm = imm_J(inst);
    cpu->regs[rd(inst)] = cpu->pc+4;
    cpu->pc += ((int32_t) imm)-4;
    
    //printf("new pc=%#-13.2x\n", cpu->pc);    
    print_op("jal\n");
}

// Performs an unconditional jump to an address specified 
// in a register and stores the return address in another register. 
void exec_JALR(CPU* cpu, uint32_t inst) {
    uint32_t imm = imm_I(inst);
    uint32_t tmp = cpu->pc;
    
    cpu->regs[rd(inst)] = tmp+4; // return addr stored in rd
    cpu->pc = ((cpu->regs[rs1(inst)] + (int32_t) imm)); // jump
    //printf("new pc=%#-13.2x\n", ((cpu->regs[rs1(inst)] + (int32_t) imm))); 
    print_op("jalr");
    if(cpu->pc == 0)
        printf(" - Returned.\n");
    else
    	printf(" - NEXT -> 0x%x, imm: 0x%x\n", cpu->pc, imm);
}

// if rs1 == rs2 jump to addr
void exec_BEQ(CPU* cpu, uint32_t inst) {
    uint32_t imm = imm_B(inst);
    if ((int32_t) cpu->regs[rs1(inst)] == (int32_t) cpu->regs[rs2(inst)])
        cpu->pc += (int32_t) imm - 4; // add offset to pc
    print_op("beq\n");
}

// if rs1 != rs2 jump to addr
void exec_BNE(CPU* cpu, uint32_t inst) {
    uint32_t imm = imm_B(inst);
    if ((int32_t) cpu->regs[rs1(inst)] != (int32_t) cpu->regs[rs2(inst)])
        cpu->pc += (int32_t) imm - 4;
    print_op("bne\n");
}

// if rs1 < rs2 jump to addr
void exec_BLT(CPU* cpu, uint32_t inst) {
    uint32_t imm = imm_B(inst);
    if ((int32_t) cpu->regs[rs1(inst)] < (int32_t) cpu->regs[rs2(inst)])
        cpu->pc += (int32_t) imm - 4;
    print_op("blt\n");
}

// if rs1 >= rs2 jump to addr
void exec_BGE(CPU* cpu, uint32_t inst) {
    uint32_t imm = imm_B(inst);
    if ((int32_t) cpu->regs[rs1(inst)] >= (int32_t) cpu->regs[rs2(inst)])
        cpu->pc += (int32_t) imm;
    print_op("bge\n");
}

// if rs1 < rs2 jump to addr
void exec_BLTU(CPU* cpu, uint32_t inst) {
    uint32_t imm = imm_B(inst);
    if (cpu->regs[rs1(inst)] < cpu->regs[rs2(inst)])
        cpu->pc += (int32_t) imm - 4;
    print_op("bltu\n");
}

// if rs1 >= rs2 jump to addr
void exec_BGEU(CPU* cpu, uint32_t inst) {
    uint32_t imm = imm_B(inst);
    if (cpu->regs[rs1(inst)] >= cpu->regs[rs2(inst)])
        cpu->pc += (int32_t) imm - 4;
    print_op("jal\n");
}

// load 1 byte to rd from address in rs1
void exec_LB(CPU* cpu, uint32_t inst) {
    uint32_t imm = imm_I(inst);
    uint32_t addr = cpu->regs[rs1(inst)] + (int32_t) imm;
    cpu->regs[rd(inst)] = (int32_t)(int8_t) cpu_load(cpu, addr, 8);
    print_op("lb\n");
}

// load 2 bytes to rd from address in rs1
void exec_LH(CPU* cpu, uint32_t inst) {
    uint32_t imm = imm_I(inst);
    uint32_t addr = cpu->regs[rs1(inst)] + (int32_t) imm;
    cpu->regs[rd(inst)] = (int32_t)(int16_t) cpu_load(cpu, addr, 16);
    print_op("lh\n");
}

// load 4 bytes to rd from address in rs1
void exec_LW(CPU* cpu, uint32_t inst) {
    uint32_t imm = imm_I(inst);
    uint32_t addr = cpu->regs[rs1(inst)] + (int32_t) imm;
    //printf("mem[%d] = %x", addr-DRAM_BASE, (int32_t) cpu_load(cpu, addr, 32));
    cpu->regs[rd(inst)] = (int32_t) cpu_load(cpu, addr, 32);
    print_op("lw\n");
}

// load 1 unsigned byte to rd from address in rs1
void exec_LBU(CPU* cpu, uint32_t inst) {
    uint32_t imm = imm_I(inst);
    uint32_t addr = cpu->regs[rs1(inst)] + (int32_t) imm;
    cpu->regs[rd(inst)] = cpu_load(cpu, addr, 8);
    print_op("lbu\n");
}

// load 2 unsigned bytes to rd from address in rs1
void exec_LHU(CPU* cpu, uint32_t inst) {
    uint32_t imm = imm_I(inst);
    uint32_t addr = cpu->regs[rs1(inst)] + (int32_t) imm;
    cpu->regs[rd(inst)] = cpu_load(cpu, addr, 16);
    print_op("lhu\n");
}

// store a single byte of data from a register into memory at a specified address
// ex: sb rs2, offset(rs1)
//   rs2 is the source register containing the byte of data to be stored.
//   offset is a 12-bit signed immediate value, representing the offset from the base address stored in register rs1.
//   rs1 is the base register containing the base address in memory where the byte will be stored.
void exec_SB(CPU* cpu, uint32_t inst) {
    uint32_t imm = imm_S(inst);
    uint32_t addr = cpu->regs[rs1(inst)] + (int32_t) imm;
    cpu_store(cpu, addr, 8, cpu->regs[rs2(inst)]);
    print_op("sb\n");
}

// same as sb but stores half-words (16 bits)
void exec_SH(CPU* cpu, uint32_t inst) {
    uint32_t imm = imm_S(inst);
    uint32_t addr = cpu->regs[rs1(inst)] + (int32_t) imm;
    cpu_store(cpu, addr, 16, cpu->regs[rs2(inst)]);
    print_op("sh\n");
}

// stores words (32 bits)
void exec_SW(CPU* cpu, uint32_t inst) {
    uint32_t imm = imm_S(inst);
    //printf("imm = %#-13.2x", imm);
    uint32_t addr = cpu->regs[rs1(inst)] + (int32_t) imm;
    //printf("addr = %#-13.2x", (addr-DRAM_BASE));
    //printf("GOING TO STORE!\n");
    cpu_store(cpu, addr, 32, cpu->regs[rs2(inst)]);
    print_op("sw\n");
}


// Adds the sign-extended 12-bit immediate to register rs1. Arithmetic overflow 
// is ignored and the result is simply the low XLEN bits of the result. 
// ADDI rd, rs1, 0 is used to implement the MV rd, rs1 assembler pseudo-instruction.
void exec_ADDI(CPU* cpu, uint32_t inst) {
    uint32_t imm = imm_I(inst);
    cpu->regs[rd(inst)] = cpu->regs[rs1(inst)] + (int32_t) imm;
    //printf("rd = %#-13.2x + %#-13.2x", cpu->regs[rs1(inst)], imm);
    print_op("addi\n");
}

// Performs logical left shift on the value in register rs1 by the shift 
// amount held in the lower 5 bits of the immediate
void exec_SLLI(CPU* cpu, uint32_t inst) {
    cpu->regs[rd(inst)] = cpu->regs[rs1(inst)] << shamt(inst);
    print_op("slli\n");
}

// Place the value 1 in register rd if register rs1 is less than the 
// signextended immediate when both are treated as signed numbers, else 0 is written to rd.
void exec_SLTI(CPU* cpu, uint32_t inst) {
    uint32_t imm = imm_I(inst);
    cpu->regs[rd(inst)] = (cpu->regs[rs1(inst)] < (int32_t) imm)?1:0;
    print_op("slti\n");
}

void exec_SLTIU(CPU* cpu, uint32_t inst) {
    uint32_t imm = imm_I(inst);
    cpu->regs[rd(inst)] = (cpu->regs[rs1(inst)] < imm)?1:0;
    print_op("sltiu\n");
}

void exec_XORI(CPU* cpu, uint32_t inst) {
    uint32_t imm = imm_I(inst);
    cpu->regs[rd(inst)] = cpu->regs[rs1(inst)] ^ imm;
    print_op("xori\n");
}

void exec_SRLI(CPU* cpu, uint32_t inst) {
    uint32_t imm = imm_I(inst);
    cpu->regs[rd(inst)] = (uint32_t)cpu->regs[rs1(inst)] >> (imm & 0x0000000f);
    print_op("srli\n");
}

void exec_SRAI(CPU* cpu, uint32_t inst) {
    uint32_t imm = imm_I(inst);
    // rd = rs1 >> imm[0:4]
    //printf("srai val: %#-13.2lx", ((int32_t)cpu->regs[rs1(inst)] >> (imm & 0x0000000f)));
    cpu->regs[rd(inst)] = (int32_t)cpu->regs[rs1(inst)] >> (imm & 0x0000000f);
    print_op("srai\n");
}

void exec_ORI(CPU* cpu, uint32_t inst) {
    uint32_t imm = imm_I(inst);
    cpu->regs[rd(inst)] = cpu->regs[rs1(inst)] | imm;
    print_op("ori\n");
}

void exec_ANDI(CPU* cpu, uint32_t inst) {
    uint32_t imm = imm_I(inst);
    cpu->regs[rd(inst)] = cpu->regs[rs1(inst)] & imm;
    print_op("andi\n");
}

void exec_ADD(CPU* cpu, uint32_t inst) {
    cpu->regs[rd(inst)] =
        (uint32_t) ((int32_t)cpu->regs[rs1(inst)] 
		 + (int32_t)cpu->regs[rs2(inst)]);
    print_op("add\n");
}

void exec_SUB(CPU* cpu, uint32_t inst) {
    cpu->regs[rd(inst)] =
        (uint32_t) ((int32_t)cpu->regs[rs1(inst)] - (int32_t)cpu->regs[rs2(inst)]);
    print_op("sub\n");
}

void exec_SLL(CPU* cpu, uint32_t inst) {
    cpu->regs[rd(inst)] = cpu->regs[rs1(inst)] << (int32_t)cpu->regs[rs2(inst)];
    print_op("sll\n");
}

void exec_SLT(CPU* cpu, uint32_t inst) {
    cpu->regs[rd(inst)] = (cpu->regs[rs1(inst)] < (int32_t) cpu->regs[rs2(inst)])?1:0;
    print_op("slt\n");
}

void exec_SLTU(CPU* cpu, uint32_t inst) {
    cpu->regs[rd(inst)] = (cpu->regs[rs1(inst)] < cpu->regs[rs2(inst)])?1:0;
    print_op("slti\n");
}

void exec_XOR(CPU* cpu, uint32_t inst) {
    cpu->regs[rd(inst)] = cpu->regs[rs1(inst)] ^ cpu->regs[rs2(inst)];
    print_op("xor\n");
}

void exec_SRL(CPU* cpu, uint32_t inst) {
    cpu->regs[rd(inst)] = cpu->regs[rs1(inst)] >> cpu->regs[rs2(inst)];
    print_op("srl\n");
}

void exec_SRA(CPU* cpu, uint32_t inst) {
    cpu->regs[rd(inst)] = (int32_t)cpu->regs[rs1(inst)] >> 
        (int32_t) cpu->regs[rs2(inst)];
    print_op("sra\n");
}

void exec_OR(CPU* cpu, uint32_t inst) {
    cpu->regs[rd(inst)] = cpu->regs[rs1(inst)] | cpu->regs[rs2(inst)];
    print_op("or\n");
}

void exec_AND(CPU* cpu, uint32_t inst) {
    cpu->regs[rd(inst)] = cpu->regs[rs1(inst)] & cpu->regs[rs2(inst)];
    print_op("and\n");
}

// ECALL & EBREAK
void exec_ECALL(CPU* cpu, uint32_t inst) {}
void exec_EBREAK(CPU* cpu, uint32_t inst) {}

void exec_ECALLBREAK(CPU* cpu, uint32_t inst) {
    if (imm_I(inst) == 0x0){
        exec_ECALL(cpu, inst); print_op("ecall\n");
    }
    if (imm_I(inst) == 0x1){
        exec_EBREAK(cpu, inst); print_op("ebreak\n");
    }
}

int cpu_execute(CPU *cpu, uint32_t inst) {
    int opcode = inst & 0x7f;           // opcode in bits 6..0
    int funct3 = (inst >> 12) & 0x7;    // funct3 in bits 14..12
    int funct7 = (inst >> 25) & 0x7f;   // funct7 in bits 31..25

    cpu->regs[0] = 0;                   // x0 hardwired to 0 at each cycle

    printf("%s\n%#.8x -> %s", ANSI_YELLOW, cpu->pc, ANSI_RESET); // DEBUG

    // TODO: Chnage debug outputs to display opcodes as binary to make 
    // identification of opcode values easier?

    switch (opcode) {
        
	case R_TYPE:  
            switch (funct3) {
                case ADDSUB:
                    switch (funct7) {
                        case ADD: exec_ADD(cpu, inst); break;
                        case SUB: exec_SUB(cpu, inst); break;
                        default: ;
                    } break;
                case SLL:  exec_SLL(cpu, inst); break;
                case SLT:  exec_SLT(cpu, inst); break;
                case SLTU: exec_SLTU(cpu, inst); break;
                case XOR:  exec_XOR(cpu, inst); break;
                case SR:   
                    switch (funct7) {
                        case SRL:  exec_SRL(cpu, inst); break;
                        case SRA:  exec_SRA(cpu, inst); break;
                        default: ;
                    } break;
                case OR:   exec_OR(cpu, inst); break;
                case AND:  exec_AND(cpu, inst); break;
                default:
                    fprintf(stderr, 
                            "[-] ERROR-> opcode:0x%x, funct3:0x%x, funct7:0x%x\n"
                            , opcode, funct3, funct7);
                    return 0;
            } break;

	case I_TYPE:  
            switch (funct3) {
                case ADDI:  exec_ADDI(cpu, inst); break;
                case SLLI:  exec_SLLI(cpu, inst); break;
                case SLTI:  exec_SLTI(cpu, inst); break;
                case SLTIU: exec_SLTIU(cpu, inst); break;
                case XORI:  exec_XORI(cpu, inst); break;
                case SRI:   
                    switch (funct7) {
                        case SRLI:  exec_SRLI(cpu, inst); break;
                        case SRAI:  exec_SRAI(cpu, inst); break;
                        default: ;
                    } break;
                case ORI:   exec_ORI(cpu, inst); break;
                case ANDI:  exec_ANDI(cpu, inst); break;
                default:
                    fprintf(stderr, 
                            "[-] ERROR-> opcode:0x%x, funct3:0x%x, funct7:0x%x\n"
                            , opcode, funct3, funct7);
                    return 0;
            } break;

	case LOAD:
            switch (funct3) {
                case LB  :  exec_LB(cpu, inst); break;  
                case LH  :  exec_LH(cpu, inst); break;  
                case LW  :  exec_LW(cpu, inst); break;  
                case LBU :  exec_LBU(cpu, inst); break; 
                case LHU :  exec_LHU(cpu, inst); break; 
                default: ;
            } break;

	case S_TYPE:
            switch (funct3) {
                case SB  :  exec_SB(cpu, inst); break;  
                case SH  :  exec_SH(cpu, inst); break;  
                case SW  :  exec_SW(cpu, inst); break;  
                default: ;
            } break;

	case B_TYPE:
            switch (funct3) {
                case BEQ:   exec_BEQ(cpu, inst); break;
                case BNE:   exec_BNE(cpu, inst); break;
                case BLT:   exec_BLT(cpu, inst); break;
                case BGE:   exec_BGE(cpu, inst); break;
                case BLTU:  exec_BLTU(cpu, inst); break;
                case BGEU:  exec_BGEU(cpu, inst); break;
                default: ;
            } break;

	case JAL:   exec_JAL(cpu, inst); break;
        case JALR:  exec_JALR(cpu, inst); break;

	case LUI:   exec_LUI(cpu, inst); break;
        case AUIPC: exec_AUIPC(cpu, inst); break;
        
        case CSR:
	    // change to switch on imm_I(inst) so that 
	    // we can differentiate between ecall and break without an extra 
	    // function 
            switch (funct3) {
                case ECALLBREAK:    exec_ECALLBREAK(cpu, inst); break;
                default:
                    fprintf(stderr, 
                            "[-] ERROR-> opcode:0x%x, funct3:0x%x, funct7:0x%x\n"
                            , opcode, funct3, funct7);
                    return 0;
            } break;

	// nop - assume execution finished. 
        case 0x00:
    	    return 0;

        default:
            fprintf(stderr, 
                    "[-] ERROR-> opcode:0x%x, funct3:0x%x, funct3:0x%x\n"
                    , opcode, funct3, funct7);
            break;
            /*exit(1);*/
    }
    if(cpu->pc == 0)
	    return 0;
    return 1;
}

void dump_registers(CPU *cpu) {
    char* abi[] = { // Application Binary Interface registers
          "x0 (zero)",  "x1 (ra)  ",   "x2 (sp)  ",   "x3 (gp)  ",
          "x4 (tp)  ",  "x5 (t0)  ",   "x6 (t1)  ",   "x7 (t2)  ",
          "x8  (s0)  ", "x9  (s1)  ",  "x10 (a0)  ",  "x11 (a1)  ",
          "x12 (a2)  ", "x13 (a3)  ",  "x14 (a4)  ",  "x15 (a5)  ",
          "x16 (a6)  ", "x17 (a7)  ",  "x18 (s2)  ",  "x19 (s3)  ",
          "x20 (s4)  ", "x21 (s5)  ",  "x22 (s6)  ",  "x23 (s7)  ",
          "x24 (s8)  ", "x25 (s9)  ",  "x26 (s10) ",  "x27 (s11) ",
          "x28 (t3)  ", "x29 (t4)  ",  "x30 (t5)  ",  "x31 (t6)  ",
    };

    // print all registers and their current values
    for (int i=0; i<8; i++) {
        printf("   %4s:  %#-13.2x  ", abi[i],    cpu->regs[i]);
        printf("   %2s:  %#-13.2x  ", abi[i+8],  cpu->regs[i+8]);
        printf("   %2s:  %#-13.2x  ", abi[i+16], cpu->regs[i+16]);
        printf("   %3s:  %#-13.2x\n", abi[i+24], cpu->regs[i+24]);
    }
}
