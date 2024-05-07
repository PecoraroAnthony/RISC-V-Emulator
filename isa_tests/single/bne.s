.global _start

_start:
  addi x28, x0, 0x04
  addi x29, x0, 0x05
  addi x30, x0, 0x05
  bne x29, x28, NotEqual # If not equal, jump to NotEqual
  jalr x0, x1, 0 # Was equal, return and exit program

NotEqual:
  addi x2, x0, 0x01
