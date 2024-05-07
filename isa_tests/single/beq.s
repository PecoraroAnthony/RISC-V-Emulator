.global _start

_start:
  addi x28, x0, 0x04
  addi x29, x0, 0x05
  addi x30, x0, 0x05
  beq x30, x29, Equal
  jalr x0, x1, 0 # indicated beq fewll through (not true)

Equal:
  addi x2, x0, 0x01

