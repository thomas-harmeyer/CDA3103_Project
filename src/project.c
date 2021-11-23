//CDA 3101 Project for Angell | Nov 23, 2021
//__ did first 5 and Thomas Harmeyer did last 5 as partners

#include "spimcore.h"


/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    // collection of if statements that check the value of ALUControl to determine desired operation, then returns the result
    if(ALUControl == 0){
        // do addition between A and B
        *ALUresult = A + B;
    }
    else if(ALUControl == 1){
        // do subtraction between A and B
        *ALUresult = A - B;
    }
    else if(ALUControl == 2){
        // set on less than command; return 1 if A < B, and if not then return 0. This one is for signed integers.
        if((signed)A < (signed)B) *ALUresult = 1;
        else ALUresult = 0;
    }
    else if(ALUControl == 3){
        // set on less than command; return 1 if A < B, and if not then return 0. This one is for unsigned integers.
        if(A < B) *ALUresult = 1;
        else *ALUresult = 0;
    }
    else if(ALUControl == 4){
        // AND statement, do AND comparison between A and B.
        *ALUresult = A & B;
    }
    else if(ALUControl == 5){
        // OR statement, do OR comparision between A and B.
        *ALUresult = A || B;
    }
    else if(ALUControl == 6){
        // shift B left 16 bits
        *ALUresult = B << 16;
    }
    else if(ALUControl == 7){
        // return NOT A
        *ALUresult = ~A;
    }
    // now check if ALUresult is == 0, if so set Zero to 1 to show this
    if(ALUresult == 0) *Zero = 1;
    else *Zero = 0;
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
    // check for word alignment, return 1 to halt if not word aligned
    if(PC % 4 != 0 || PC > 65536) return 1;
    // properly aligned, now set up instruction and return 0 to keep going
    else{
        *instruction = Mem[PC>>2];
        return 0;
    }
}


/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
    // use masking to get the bits we need for all the variables
    *op = (instruction & 0xFC000000) >> 26;
    *r1 = (instruction & 0x3E00000) >> 26;
    *r2 = (instruction & 0x1F0000) >> 26;
    *r3 = (instruction & 0xF800) >> 26;
    *funct = (instruction & 0x3F);
    *offset = (instruction & 0xFFFF);
    *jsec = (instruction & 0x3FFFFFF);
}



/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{
    // set initial values to 0
    controls->ALUOp = 0;
    controls->ALUSrc = 0;
    controls->Branch = 0;
    controls->Jump = 0;
    controls->MemRead = 0;
    controls->MemtoReg = 0;
    controls->MemWrite = 0;
    controls->RegDst = 0;
    controls->RegWrite = 0;

    //check op to get desired operation
    if(op == 0x0){
        // set up controls for r-type instructions
        controls->ALUOp = 0x7;
        controls->RegDst = 1;
        controls->RegWrite = 1;
    }
    else if(op == 0x08){
        // set up controls for add immediate
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
    }
    else if(op == 0x23){
        // set up controls for load word
        controls->ALUSrc = 0x1;
        controls->MemRead = 0x1;
        controls->MemtoReg = 0x1;
        controls->MemWrite = 0x1;
    }
    else if(op == 0x2B){
        // set up controls for save word
        controls->ALUSrc = 0x1;
        controls->MemtoReg = 2;
        controls->MemWrite = 0x1;
        controls->RegDst = 2;
    }
    else if(op == 0x04){
        // set up controls for beq
        controls->ALUOp = 0x1;
        controls->Branch = 1;
        controls->MemtoReg = 2;
        controls->RegDst = 2;
    }
    else if(op == 0x02){
        // set up controls for jump operation
        controls->ALUOp = 2;
        controls->ALUSrc = 2;
        controls->Jump = 1;
        controls->MemtoReg = 2;
        controls->RegDst = 2;
    }
    else if(op == 0x0A){
        // set up controls for set on less than immediate
        controls->ALUOp = 2;
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
    }
    else if(op == 0x0B){
        // sets the controls for sliu
        controls->ALUOp = 3;
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
    }
    else if(op == 0x0F){
        // sets up the controls for lui
        controls->ALUOp = 6;
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
    }
    // if we get here, then we have an invalid command. halt the program.
    else return 1;
    // we have a valid command, return 0 to keep going
    return 0;
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
    // read the data from the registers and store them into data1 and data2
    *data1 = Reg[r1];
    *data2 = Reg[r2];
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
