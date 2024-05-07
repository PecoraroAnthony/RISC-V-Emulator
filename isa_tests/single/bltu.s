.global _start

_start:
  addi x28, x0, 0x04
  addi x29, x0, 0x05
  addi x30, x0, 0x05
  bltu x30, x29, True # Jump if rs1 < rs2
  jalr x0, x1, 0 # rs1 > rs2; return and end prog 

True:
  addi x2, x0, 0x01
