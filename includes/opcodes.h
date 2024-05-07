// opcodes.h
#ifndef OPCODES_H
#define OPCODES_H

#define R_TYPE  0x33 // 0b0110011
    #define ADDSUB  0x0
        #define ADD     0x00
        #define SUB     0x20
    #define SLL     0x1
    #define SLT     0x2
    #define SLTU    0x3
    #define XOR     0x4
    #define SR      0x5
        #define SRL     0x00
        #define SRA     0x20
    #define OR      0x6
    #define AND     0x7

#define I_TYPE  0x13 // 0b0010011
    #define ADDI    0x0
    #define SLLI    0x1
    #define SLTI    0x2
    #define SLTIU   0x3
    #define XORI    0x4
    #define SRI     0x5
        #define SRLI    0x00
        #define SRAI    0x20
    #define ORI     0x6
    #define ANDI    0x7

#define LOAD    0x03 // 0b0000011
    #define LB      0x0
    #define LH      0x1
    #define LW      0x2
    #define LBU     0x4
    #define LHU     0x5

#define S_TYPE  0x23 // 0b0100011
    #define SB      0x0
    #define SH      0x1
    #define SW      0x2

#define B_TYPE  0x63 // 0b1100011
    #define BEQ     0x0
    #define BNE     0x1
    #define BLT     0x4
    #define BGE     0x5
    #define BLTU    0x6
    #define BGEU    0x7

#define JAL     0x6f // 0b1101111
#define JALR    0x67 // 0b1100111

#define LUI     0x37 // 0b0110111 
#define AUIPC   0x17 // 0b0010111

#define CSR 0x73 // 0b1110011
    #define ECALLBREAK    0x00     // contains both ECALL and EBREAK

#endif
