#!/usr/bin/env python

#######################################################################
# Python Script to Convert All C Programs into RISCV Binary Executables
######################################################################
import sys
import re
import os

def build_bin(source_dir, obj_dir, asm_dir, dest_dir):
    # print(obj_dir)
    # print(asm_dir)
    # print(dest_dir)
    files = [f for f in os.listdir(source_dir) 
            if (os.path.isfile(os.path.join(source_dir, f)) 
                & (".dump" not in f)
                & ("Make" not in f)
                & ("git" not in f)
                )]
    #print(files)

    for f in files:
        f = os.path.join(source_dir, f)
        filename = f.split("/")[-1]
        objname = os.path.splitext(filename)[0]
        print(filename, end="\n")
        #print(objname)
        os.system("riscv32-unknown-elf-gcc -S " 
                  + source_dir + filename + " -o " + asm_dir + objname + ".s")

        os.system("riscv32-unknown-elf-gcc -Wl,-Ttext=0x0 -nostdlib -march=rv32i -mabi=ilp32 -o " + 
                  obj_dir + objname + " " + asm_dir + objname + ".s") 
        
        os.system("riscv32-unknown-elf-objcopy -O binary " +
                  obj_dir + objname + " " + dest_dir + objname + ".bin")

if __name__ == "__main__":
    if len(sys.argv) > 1:
        print ("Usage: ./build_cprog.py")
        exit(1)
    print("Creating .bin files in ./isa_tests/cprog/bin/")
    source_dir = "./isa_tests/cprog/"
    obj_dir = "./isa_tests/cprog/obj/"
    asm_dir = "./isa_tests/cprog/asm/"
    dest_dir = "./isa_tests/cprog/bin/"
    build_bin(source_dir, obj_dir, asm_dir, dest_dir)
