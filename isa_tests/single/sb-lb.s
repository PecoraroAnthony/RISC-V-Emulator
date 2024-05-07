.global _start

_start: 
  li x5, 0x2a   # load x5 with value
  
  sb x5, 0x1c(sp) # Store the byte value from t0 
		# into memory at the address pointed by sp
		# plus an offset of 28 (0x8001000 + 0x1c)

  lb x6, 0x1c(sp)    # Load the byte from memory at the address 
		   # pointed by sp into register t1 

  jalr x0, x1, 0   # return
