# ------------------------------------------------------------------------------------
# [KNU COMP0411 Computer Architectures] Skeleton code for the 1st project (calculator)
# ------------------------------------------------------------------------------------
.data
buffer: .space 100
howToExit: .ascii "Input 'Enter' key only to exit the program.\0"

.text
# main
main:
	li a5, 1	# print or not(DO NOT COMMENT THIS)
	
	jal x1, test #functionality test (please comment this line out if you want console test)
	
	# print how to exit(enter)
	la a0, howToExit
	li a7, 4
	ecall

	while:
		#print " > "
		li a7, 11
		
		li a0, 10  # enter
		ecall
		li a0, 32  # space
		ecall
		li a0, 62  # >
		ecall
		li a0, 32  # space
		ecall
		
		#input string
		la a0, buffer
		li a1, 100
		li a7, 8
		ecall
		
		add t5, a0, zero  # t5 = a0
		
		# exit check
		li t6, 10
		lbu t0, 0(t5)
		beq t0, t6, exitWithEnter
		
		#print space
		li a7, 11
		li a0, 32
		ecall
		
		# arithmetic operations
		li a2, 0	# fisrt operand
		li a3, 0	# second operand
		li a6, 0	# 0 for digit1, 1 for digit2
		
		loop:
			lbu t0, 0(t5)
			
			# if EOF
			li t6, 10
			beq t0, t6, myCallCalc
			
			# print char
			add a0, t0, zero
			li a7, 11
			ecall
			
			# if +
			li t6, 43
			beq t0, t6, mySetAdd
			
			# if -
			li t6, 45
			beq t0, t6, mySetSub
			
			# if *
			li t6, 42
			beq t0, t6, mySetMul
			
			# if /
			li t6, 47
			beq t0, t6, mySetDiv
			
			# if number
			addi t0, t0, -48
			beq a6, zero, myDigit1
			
			myDigit2:
				slli t1, a3, 1
				slli a3, a3, 3
				add a3, a3, t1
				add a3, a3, t0
				j myNext
			
			myDigit1:
				slli t1, a2, 1
				slli a2, a2, 3
				add a2, a2, t1
				add a2, a2, t0
			
			myNext:
				addi t5, t5, 1
				j loop
				
		mySetAdd:
			li a1, 0
			li a6, 1
			j myNext
			
		mySetSub:
			li a1, 1
			li a6, 1
			j myNext
			
		mySetMul:
			li a1, 2
			li a6, 1
			j myNext
			
		mySetDiv:
			li a1, 3
			li a6, 1
			j myNext
			
		myCallCalc:
			li a7, 11
			li a0, 61
			ecall
			li a5, 0
			jal x1, calc
		
	j while
	

#---------------------------------------------------------------------------------------------
#name: calc
#func: performs arithmetic operation
#x11(a1): arithmetic operation (0: addition, 1:  subtraction, 2:  multiplication, 3: division)
#x12(a2): the first operand
#x13(a3): the second operand
#x10(a0): return value
#x14(a4): return value (remainder for division operation)
#---------------------------------------------------------------------------------------------
calc:
	beq a1, zero, myAdd
	
	li t0, 1
	beq a1, t0, mySub
	
	li t0, 2
	beq a1, t0, myMul
	
	li t0, 3
	beq a1, t0, myDiv	
	
	jalr x0, 0(x1)
	
	
myAdd:
	add a0, a2, a3
	
	beq a5, zero, myPrint
	
	jalr x0, 0(x1)

mySub:
	neg t3, a3
	add a0, a2, t3
	
	beq a5, zero, myPrint
	
	jalr x0, 0(x1)

myMul:
	li t1 32
	li a0, 0
	addi t2, a2, 0	#multiplicand
	addi t3, a3, 0	#multiplier

	
	myCompare:
		beq t1, zero, myMulExit
		andi t5, t3, 1
		beq t5, zero, myShift
		add a0, t2, a0
		
	myShift:
		slli t2, t2, 1
		srli t3, t3, 1
		addi t1, t1, -1
		j myCompare
		
	myMulExit:
		beq a5, zero, myPrint
		
		jalr x0, 0(x1)


myDiv:
	li t1, 16
	li a4, 0	# remainder
	li a0, 0	# quotient
	addi t2, a2, 0	# dividend
	addi t3, a3, 0	# divisor
	slli t3, t3, 15
	
	myCal:
		beq t1, zero, myDivExit
		addi t1, t1, -1
		
		neg t4, t3
		add t2, t2, t4

		blt t2, zero, myNegative
		bge t2, zero, myPositive
		
		
	myNegative:
		add t2, t2, t3
		slli a0, a0, 1
		j myDivShift
		
	myPositive:
		slli a0, a0, 1
		addi a0, a0, 1
		j myDivShift
	
	myDivShift:
		srli t3, t3, 1
		
		j myCal
		
	myDivExit:
		add a4, t2, zero
		beq a5, zero, myPrintDiv
		
		jalr x0, 0(x1)

myPrint:
	li a7, 1
	ecall
	
	jalr x0, 0(x1)

myPrintDiv:
	li a7, 1
	ecall
	
	li a7, 11
	li a0, 44
	ecall
	
	add a0, a4, zero
	li a7, 1
	ecall
	
	jalr x0, 0(x1)

exitWithEnter:
        li a0, 0
        li a7, 93
        ecall
        ebreak

.include "common.asm"
