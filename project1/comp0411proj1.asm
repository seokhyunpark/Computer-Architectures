# ------------------------------------------------------------------------------------
# [KNU COMP0411 Computer Architectures] Skeleton code for the 1st project (calculator)
# ------------------------------------------------------------------------------------

.text	
# main
main:
	
	jal x1, test #functionality test (please comment this line out if you want console test)
	
	#----TODO-------------------------------------------------------
	#1. read a string from the console
	#2. perform arithmetic operations
	#3. print a string to the console to show the computation result
	#---------------------------------------------------------------
	
	# Exit (93) with code 0
        li a0, 0
        li a7, 93
        ecall
        ebreak


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
	#TODO
	
	
	
	
	
	jalr x0, 0(x1)


.include "common.asm"
