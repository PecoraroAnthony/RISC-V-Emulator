#include "../includes/dram.h"
#include <stdio.h>

// Because memory locations are indexed by byte, 
// a bitwise concatenation is performed from the start 
// of memory [addr-DRAM_BASE] with that memory address indexed
// by one byte. This is done once for 16 bit values and 3 times for 
// 32 bit values. 
uint32_t dram_load_8(DRAM* dram, uint32_t addr){
    return (uint32_t) dram->mem[addr - DRAM_BASE];
}
uint32_t dram_load_16(DRAM* dram, uint32_t addr){
    return (uint32_t) dram->mem[addr-DRAM_BASE]
        |  (uint32_t) dram->mem[addr-DRAM_BASE + 1] << 8;
}
uint32_t dram_load_32(DRAM* dram, uint32_t addr){
    return (uint32_t) dram->mem[addr-DRAM_BASE]
        |  (uint32_t) dram->mem[addr-DRAM_BASE + 1] << 8
        |  (uint32_t) dram->mem[addr-DRAM_BASE + 2] << 16
        |  (uint32_t) dram->mem[addr-DRAM_BASE + 3] << 24;
}

// size of data can be 8,16,32, or 64 bits in length
uint32_t dram_load(DRAM* dram, uint32_t addr, uint32_t size) {
    switch (size) {
        case 8:  return dram_load_8(dram, addr);  break;
        case 16: return dram_load_16(dram, addr); break;
        case 32: return dram_load_32(dram, addr); break;
        default: ;
    }
    return 1; // attempting to load illegal size
}

void dram_store_8(DRAM* dram, uint32_t addr, uint32_t value) {
    dram->mem[addr-DRAM_BASE] = (uint8_t) (value & 0xff);
}
void dram_store_16(DRAM* dram, uint32_t addr, uint32_t value) {
    dram->mem[addr-DRAM_BASE] = (uint8_t) (value & 0xff);
    dram->mem[addr-DRAM_BASE+1] = (uint8_t) ((value >> 8) & 0xff);
}
void dram_store_32(DRAM* dram, uint32_t addr, uint32_t value) { 
    //printf("addr=%#-13.2x", addr);	
    dram->mem[addr-DRAM_BASE] = (uint8_t) (value & 0xff);
    //printf("dram->mem[addr-DRAM_BASE] = %x at addr =%x \n", dram->mem[addr-DRAM_BASE], addr-DRAM_BASE);
    dram->mem[addr-DRAM_BASE + 1] = (uint8_t) ((value >> 8) & 0xff);
    dram->mem[addr-DRAM_BASE + 2] = (uint8_t) ((value >> 16) & 0xff);
    dram->mem[addr-DRAM_BASE + 3] = (uint8_t) ((value >> 24) & 0xff);
}
void dram_store(DRAM* dram, uint32_t addr, uint32_t size, uint32_t value) { 
    //printf("DRAM_STORE: addr=%13.2x, size=%d, value=%#-13.2x", addr, size, value);
    switch (size) {
        case 8:  dram_store_8(dram, addr, value); break;
        case 16: dram_store_16(dram, addr, value); break;
        case 32: dram_store_32(dram, addr, value); break;
        default: printf("default reached in dram store\n");
    }
}
