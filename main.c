/**************
 * Main.c for RISCV-EMULATOR Capstone Project
 *   Chico State Spring 2024
 * Anthony Pecoraro
 **************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "includes/cpu.h"

void read_file(CPU* cpu, char *filename)
{
    FILE *file;
    uint8_t *buffer;
    unsigned long fileLen;

    // open file
    file = fopen(filename, "rb");
    if (!file)
    {
	fprintf(stderr, "Unable to open file %s", filename);
    }  

    //Get file length
    fseek(file, 0, SEEK_END); // move pos pointer to end of file
    fileLen=ftell(file);      // return pos pointer location and set as filelen 
    fseek(file, 0, SEEK_SET); // set pos pointer back to beginning of file 

    //Allocate memory
    buffer=(uint8_t *)malloc(fileLen+1);
    if (!buffer)
    {
	fprintf(stderr, "Memory Allocation Error.");
        fclose(file);
    }

    // Read file contents into buffer
    fread(buffer, fileLen, 1, file);
    fclose(file);

    // copy the bin executable to dram
    memcpy(cpu->bus.dram.mem, buffer, fileLen*sizeof(uint8_t));
	free(buffer);
}

int main(int argc, char* argv[]) {
    int dr = 0; // enable reg dump: 0=no, 1=yes
    if (argc == 2 || argc == 3) {
	if(argc == 3){
	    if(strcmp(argv[2], "-d")){
	        printf("main invalid option -- \'%s\'\n", argv[2]);
	        printf("Usage: ./main <filename> [-d dump registers]\n");
		exit(-1);
	    } else { dr = 1;}
	}
    }else {
        printf("Usage: ./main <filename> [-d dump registers]\n");
	exit(-1);
    }

    // Initialize cpu, registers and program counter
    struct CPU cpu;
    cpu_init(&cpu);
    // Read input file
    read_file(&cpu, argv[1]);

    // cpu clock cycle loop
    while (1) {
        // fetch instruction at current pc
        uint32_t inst = cpu_fetch(&cpu);

	// execute instr
        if (!cpu_execute(&cpu, inst))
            break;

	// Increment the program counter		
        cpu.pc += 4; 

        if(dr) dump_registers(&cpu);

	// if the pc is set to 0 at any time, end program
        if(cpu.pc==0x00){
	    printf("pc==0. Execution finished.\n");
   	    break;
	}
    }
    //dump_registers(&cpu);
    printf("pc reached end of instruction. Execution finished.\n");
    return 0;
}
