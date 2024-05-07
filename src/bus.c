/*******
 * src/bus.c
 *******/
#include "../includes/bus.h"
#include <stdio.h>

uint32_t bus_load(BUS* bus, uint32_t addr, uint32_t size) {
    // load from mem the value found at the given address with given size
    return dram_load(&(bus->dram), addr, size);
}

void bus_store(BUS* bus, uint32_t addr, uint32_t size, uint32_t value) {
    //printf("\nBUS STORE: addr=%#-13.2x ,size=%d, value=%#-13.2x\n", addr, size, value);
    // store into memory at current address the given value with given size
    dram_store(&(bus->dram), addr, size, value);
}
