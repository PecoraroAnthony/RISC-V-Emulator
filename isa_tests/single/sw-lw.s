.global _start

_start:
  li x5, 0x2a2a2a2a  # Load word (4 bytes) into x5

  sw x5, 0x0c(sp)    # Store the word in memory

  lw x6, 0x0c(sp)    # Load the word from memory at the address
                   # pointed by sp into register x6

  jalr x0, x1, 0   # return
