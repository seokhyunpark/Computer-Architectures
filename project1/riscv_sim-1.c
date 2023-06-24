/*
컴퓨터구조 COMP0411-002
Project 2. RISC-V Simulator
2020110973 박석현
*/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>

unsigned long current_inst = 0;
unsigned long opcode, rd, rs1, rs2, funct3, funct7, imm1, imm2;
unsigned long alu_result;
unsigned long imm_t1, imm_t2, imm_t3, imm_t4;

//clock cycles
long long cycles = 0;

//registers
long long int regs[32];

//program counter
unsigned long pc = 0;
unsigned long temp_pc = 0;

//memory
#define INST_MEM_SIZE 32*1024
#define DATA_MEM_SIZE 32*1024
unsigned long inst_mem[INST_MEM_SIZE]; //instruction memory
unsigned long long data_mem[DATA_MEM_SIZE]; //data memory

//misc. function
int init(char* filename);

void print_cycles();
void print_reg();
void print_pc();

//set regs[0] = 0
void regs_zero();

//fetch an instruction from a instruction memory
void fetch() { 
	pc /= 4;
	current_inst = inst_mem[pc];
	pc *= 4;
	// printf("current_inst = %lx, [%d]\n", current_inst, pc);
}

//decode the instruction and read data from register file
void decode() {
	opcode = current_inst & 0x7F;

	switch (opcode)
	{
		case 0x33: //R-type (add, sub, xor, or, and)
			rd = (current_inst >> 7) & 0x1F;
			funct3 = (current_inst >> 12) & 0x7;
			rs1 = (current_inst >> 15) & 0x1F;
			rs2 = (current_inst >> 20) & 0x1F;
			funct7 = (current_inst >> 25) & 0x7F;
			break;
		case 0x13: //I-type (addi, xori, ori, andi)
			rd = (current_inst >> 7) & 0x1F;
			funct3 = (current_inst >> 12) & 0x7;
			rs1 = (current_inst >> 15) & 0x1F;
			imm1 = (current_inst >> 20) & 0x7FF;
			break;
		case 0x3: //I-type (lw)
			rd = (current_inst >> 7) & 0x1F;
			funct3 = (current_inst >> 12) & 0x7;
			rs1 = (current_inst >> 15) & 0x1F;
			imm1 = (current_inst >> 20) & 0xFFF;
			break;
		case 0x67: //I-type (jalr)
			rd = (current_inst >> 7) & 0x1F;
			funct3 = (current_inst >> 12) & 0x7;
			rs1 = (current_inst >> 15) & 0x1F;
			imm1 = (current_inst >> 20) & 0xFFF;
			break;
		case 0x23: //S-type (sw)
			funct3 = (current_inst >> 12) & 0x7;
			imm1 = (current_inst >> 7) & 0x1F;
			rs1 = (current_inst >> 15) & 0x1F;
			rs2 = (current_inst >> 20) & 0x1F;
			imm2 = (current_inst >> 25) & 0x7F;
			break;
		case 0x63: //SB-type (beq)
			funct3 = (current_inst >> 12) & 0x7;
			imm1 = (current_inst >> 7) & 0x1F;
			rs1 = (current_inst >> 15) & 0x1F;
			rs2 = (current_inst >> 20) & 0x1F;
			imm2 = (current_inst >> 25) & 0x7F;
			break;
		case 0x6F: //UJ-type (jal)
			rd = (current_inst >> 7) & 0x1F;
			imm1 = (current_inst >> 12) & 0xFFFFF;
			break;
		default:
			break;
	}
}

//perform the appropriate operation 
void exe() {
	switch (opcode)
	{
		case 0x33: //R-type (add, sub, xor, or, and)
			switch (funct3)
			{
				case 0x0: //add, sub
					if (funct7 == 0x0) //add
						alu_result = regs[rs1] + regs[rs2];
					else //sub
						alu_result = regs[rs1] - regs[rs2];
					break;
				case 0x4: //xor
					alu_result = regs[rs1] ^ regs[rs2];
					break;
				case 0x6: //or
					alu_result = regs[rs1] | regs[rs2];
					break;
				case 0x7: //and
					alu_result = regs[rs1] & regs[rs2];
					break;
				default:
					break;
			}
			break;
		case 0x13: //I-type (addi, xori, ori, andi)
			switch (funct3)
			{
				case 0x0: //addi
					if ((current_inst >> 20) & 0x800) {
						imm1 = ~imm1 + 1;
						imm1 = imm1 & 0x7FF;
						alu_result = regs[rs1] - imm1;
					}
					else {
						alu_result = regs[rs1] + imm1;
					}
					break;
				case 0x4: //xori
					alu_result = regs[rs1] ^ imm1;
					break;
				case 0x6: //ori
					alu_result = regs[rs1] | imm1;
					break;
				case 0x7: //andi
					alu_result = regs[rs1] & imm1;
					break;
				default:
					break;
			}
			break;
		case 0x3: //I-type (lw)
			alu_result = regs[rs1] + imm1;
			break;
		case 0x67: //I-type (jalr)
			temp_pc = pc + 4;
			pc = regs[rs1 + imm1];
			break;
		case 0x23: //S-type (sw)
			alu_result = regs[rs1] + imm1;
			break;
		case 0x63: //SB-type (beq)
			if (regs[rs1] == regs[rs2]) {
					imm1 |= (imm2 << 5);
					imm_t1 = (imm1 >> 11) & 0x1;
					imm_t2 = (imm1 & 0xff);
					imm_t3 = (imm1 >> 8) & 0x1;
					imm_t4 = (imm1 >> 9) & 0x3ff;
				if ((imm_t1) & 0x1) {
					imm1 = ~imm1 + 1;
					imm1 = imm1 & 0x7FF;
					pc -= imm1;
				}
				else {
					pc += imm1;
				}
			}
			else {
				pc += 4;
			}
			break;
		case 0x6F: //UJ-type (jal)
			imm_t1 = (imm1 >> 19) & 0x1;
			imm_t2 = (imm1 & 0xff);
			imm_t3 = (imm1 >> 8) & 0x1;
			imm_t4 = (imm1 >> 9) & 0x3ff;
			imm1 = (imm_t1 << 19) | (imm_t2 << 11) | (imm_t3 << 10) | imm_t4;
			imm1 <<= 1;

			temp_pc = pc + 4;
			if ((imm_t1) & 0x1) {
				imm1 = ~imm1 + 1;
				imm1 = imm1 & 0x7FF;
				pc -= imm1;
			}
			else {
				pc += imm1;
			}
			break;
		default:
			break;
	}
}

//access the data memory
void mem() {
	switch (opcode)
	{
		case 0x3: //I-type (lw)
			regs[rd] = data_mem[alu_result];
			break;
		case 0x23: //S-type (sw)
			data_mem[alu_result] = regs[rs2];
			break;
		default:
			break;
	}
}

//write result of arithmetic operation or data read from the data memory if required
void wb() {
	switch (opcode)
	{
		case 0x33: //R-type (add, sub, xor, or, and)
			regs[rd] = alu_result;
			pc += 4;
			break;
		case 0x13: //I-type (addi, xori, ori, andi)
			regs[rd] = alu_result;
			pc += 4;
			break;
		case 0x3: //I-type (lw)
			pc += 4;
			break;
		case 0x67: //I-type (jalr)
			regs[rd] = temp_pc;
			break;
		case 0x23: //S-type (sw)
			pc += 4;
			break;
		case 0x6F: //UJ-type (jal)
			regs[rd] = temp_pc;
			break;
	}
	regs_zero();
}

int main(int ac, char* av[])
{

	if (ac < 3)
	{
		printf("./riscv_sim filename mode\n");
		return -1;
	}

	char done = 0;
	if (init(av[1]) != 0)
		return -1;
	while (!done)
	{
		fetch();
		decode();
		exe();
		mem();
		wb();
		// sleep(1);

		cycles++;    //increase clock cycle

		//if debug mode, print clock cycle, pc, reg 
		if (*av[2] == '0') {
			print_cycles();  //print clock cycles
			print_pc();		 //print pc
			print_reg();	 //print registers
		}

		// check the exit condition, do not delete!! 
		if (regs[9] == 10)  //if value in $t1 is 10, finish the simulation
			done = 1;
	}

	if (*av[2] == '1')
	{
		print_cycles();  //print clock cycles
		print_pc();		 //print pc
		print_reg();	 //print registers
	}

	return 0;
}


/* initialize all datapat elements
//fill the instruction and data memory
//reset the registers
*/
int init(char* filename)
{
	FILE* fp = fopen(filename, "r");
	int i;
	long inst;

	if (fp == NULL)
	{
		fprintf(stderr, "Error opening file.\n");
		return -1;
	}

	/* fill instruction memory */
	i = 0;
	while (fscanf(fp, "%lx", &inst) == 1)
	{
		inst_mem[i++] = inst;
	}


	/*reset the registers*/
	for (i = 0; i < 32; i++)
	{
		regs[i] = 0;
	}

	/*reset pc*/
	pc = 0;
	/*reset clock cycles*/
	cycles = 0;
	return 0;
}

void print_cycles()
{
	printf("---------------------------------------------------\n");

	printf("Clock cycles = %lld\n", cycles);
}

void print_pc()
{
	printf("PC	   = %ld\n\n", pc);
}

void regs_zero() {
	regs[0] = 0;
}

void print_reg()
{
	printf("x0   = %d\n", regs[0]);
	printf("x1   = %d\n", regs[1]);
	printf("x2   = %d\n", regs[2]);
	printf("x3   = %d\n", regs[3]);
	printf("x4   = %d\n", regs[4]);
	printf("x5   = %d\n", regs[5]);
	printf("x6   = %d\n", regs[6]);
	printf("x7   = %d\n", regs[7]);
	printf("x8   = %d\n", regs[8]);
	printf("x9   = %d\n", regs[9]);
	printf("x10  = %d\n", regs[10]);
	printf("x11  = %d\n", regs[11]);
	printf("x12  = %d\n", regs[12]);
	printf("x13  = %d\n", regs[13]);
	printf("x14  = %d\n", regs[14]);
	printf("x15  = %d\n", regs[15]);
	printf("x16  = %d\n", regs[16]);
	printf("x17  = %d\n", regs[17]);
	printf("x18  = %d\n", regs[18]);
	printf("x19  = %d\n", regs[19]);
	printf("x20  = %d\n", regs[20]);
	printf("x21  = %d\n", regs[21]);
	printf("x22  = %d\n", regs[22]);
	printf("x23  = %d\n", regs[23]);
	printf("x24  = %d\n", regs[24]);
	printf("x25  = %d\n", regs[25]);
	printf("x26  = %d\n", regs[26]);
	printf("x27  = %d\n", regs[27]);
	printf("x28  = %d\n", regs[28]);
	printf("x29  = %d\n", regs[29]);
	printf("x30  = %d\n", regs[30]);
	printf("x31  = %d\n", regs[31]);
	printf("\n");
}