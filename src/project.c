#include "spimcore.h"


/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{

}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{

}


/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{

}



/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{

}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{

}

/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset, unsigned *extended_value)
{
	if ((offset >> 15))						   //check sign bit
		*extended_value = offset | 4294901760; //change bits to keep sign value
	else
		*extended_value = offset & 65535; //change bits to keep sign value
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1, unsigned data2, unsigned extended_value, unsigned funct, char ALUOp, char ALUSrc, unsigned *ALUresult, char *Zero)
{
	// call alu
	unsigned data2_temp;

	if (!ALUSrc)
	{
		data2_temp = data2;
	}
	else
	{
		data2_temp = extended_value;
	}

	unsigned char aluRule = ALUOp;
	if (ALUOp > 6)
		switch (ALUOp)
		{
		case 7: //r instruction
			switch (funct)
			{
			default: // command will cause halt
				return 1;
			case 27: // ||
				aluRule = 5;
				break;
			case 32: // +
				aluRule = 0;
				break;
			case 36: // &
				aluRule = 4;
				break;
			case 42: // slt
				aluRule = 2;
				break;
			case 43: // sltu
				aluRule = 3;
				break;
			}
			break;
		default:
			return 1;
		}

	ALU(data1, data2_temp, aluRule, ALUresult, Zero); //run alu

	return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult, unsigned data2, char MemWrite, char MemRead, unsigned *memdata, unsigned *Mem)
{
	if (MemRead) //if MemRead
	{
		if ((!(ALUresult % 4)) && ALUresult < 65536) //check word alignedness
			*memdata = Mem[ALUresult >> 2];			 //extract word from memory
		else
			return 1; //otherwise return 1
	}

	if (MemWrite == 1) //if MemWrite
	{
		if ((!(ALUresult % 4)) && ALUresult < 65536) //check word alignedness
			Mem[ALUresult >> 2] = data2;			 //write word to memory
		else
			return 1; //otherwise return 1
}

	return 0;
}

/* Write Register */
/* 10 Points */
void write_register(unsigned r2, unsigned r3, unsigned memdata, unsigned ALUresult, char RegWrite, char RegDst, char MemtoReg, unsigned *Reg)
{
	if (RegWrite) //if RegWrite
	{
		if (MemtoReg) //if MemtoReg
		{
			if (RegDst)			   //if RegDst
				Reg[r3] = memdata; //write to register
			else
				Reg[r2] = memdata; //write to register
		}
		else
		{
			if (RegDst)				 //if RegDst
				Reg[r3] = ALUresult; //write to register
			else
				Reg[r2] = ALUresult; //write to register
		}
	}
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec, unsigned extended_value, char Branch, char Jump, char Zero, unsigned *PC)
{
	*PC += 4; //inct PC by 4

	if (Zero && Branch)					   //if zero or branch
		*PC = *PC + (extended_value << 2); //update PC

	if (Jump)									//if jump
		*PC = (*PC & 4026531840) | (jsec << 2); //update PC with jsec
}
