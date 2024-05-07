// dram.h
// Memory-mapped I/O as per RISCV Architecture Convention
// - lower addresses used for I/O ports 
// - dram mem starts at  0x80000000 = DRAM_BASE

#include <stdint.h>

#define DRAM_SIZE 1024*1024*1     // 1 MiB DRAM = 0x100000 bytes
#define DRAM_BASE 0x80000000

typedef struct DRAM {
    uint8_t mem[DRAM_SIZE];     // Dram memory of DRAM_SIZE
} DRAM;

uint32_t dram_load(DRAM* dram, uint32_t addr, uint32_t size);

void dram_store(DRAM* dram, uint32_t addr, uint32_t size, uint32_t value);
