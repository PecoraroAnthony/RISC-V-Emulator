.global _start

_start:
  li x5, 0x2a2a      # 8 bytes (0x2a2a) into x5
		     # The cross-compiler does the work in 
		     # determining how to perform the psuedo-instruction

  sh x5, 28(sp) # Store the 2 bytes in x5
                # into memory at the address pointed by sp
                # plus an offset of 28 (0x8001000 + 0x1c)

  lh x6, 28(sp)    # Load the 2 bytes from memory at the address
                   # pointed by sp into register t1

  jalr x0, x1, 0   # return
