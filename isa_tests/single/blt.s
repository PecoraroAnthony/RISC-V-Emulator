.global _start

_start:
  addi x28, x0, 0x04
  addi x29, x0, 0x05
  addi x30, x0, 0x05
  blt x28, x30, True # Jump if rs1 < rs2
  jalr x0, x1, 0 # rs1 !< rs2

True:
  addi x2, x0, 0x01
