.global _start

_start:
  addi x28, x0, 0x04
  addi x29, x0, 0x05
  jal x30, Equal # jump to Equal and store return addr in x30 
  addi x31, x0, 0x15

Equal:
  addi x2, x0, 0x01
