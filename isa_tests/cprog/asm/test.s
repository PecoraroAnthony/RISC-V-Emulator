	.file	"test.c"
	.option nopic
	.attribute arch, "rv32i2p1_m2p0_a2p1_f2p2_d2p2_zicsr2p0_zifencei2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.align	2
	.globl	main
	.type	main, @function
main:
	addi	sp,sp,-32
	sw	s0,28(sp)
	addi	s0,sp,32
	li	a5,10
	sw	a5,-20(s0)
	lw	a5,-20(s0)
	addi	a5,a5,-11
	mv	a0,a5
	lw	s0,28(sp)
	addi	sp,sp,32
	jr	ra
	.size	main, .-main
	.align	2
	.globl	fact
	.type	fact, @function
fact:
	addi	sp,sp,-32
	sw	ra,28(sp)
	sw	s0,24(sp)
	addi	s0,sp,32
	sw	a0,-20(s0)
	lw	a4,-20(s0)
	li	a5,1
	bne	a4,a5,.L4
	lw	a5,-20(s0)
	j	.L5
.L4:
	lw	a5,-20(s0)
	addi	a5,a5,-1
	mv	a0,a5
	call	fact
	mv	a4,a0
	lw	a5,-20(s0)
	mul	a5,a4,a5
.L5:
	mv	a0,a5
	lw	ra,28(sp)
	lw	s0,24(sp)
	addi	sp,sp,32
	jr	ra
	.size	fact, .-fact
	.ident	"GCC: (gc891d8dc23e) 13.2.0"
