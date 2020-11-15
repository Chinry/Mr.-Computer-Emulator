#include "m6502.h"
#include "opcodes.h"
#include <stdlib.h>


#define _ORA_ 0
#define _AND_ 1
#define _EOR_ 2
#define _ADC_ 3
#define _STA_ 4
#define _LDA_ 5
#define _CMP_ 6
#define _SBC_ 7



#define opcode1(opcode_bits,function,access)\
	lookup[(opcode_bits << 5) + 1].func = &address;\
	op[op_index].reg = REG_A;\
	op[op_index].RW = access;\
	op[op_index].addressing = INDEXED_INDIRECT;\
	op[op_index].opcode_func = &function;\
	lookup[(opcode_bits << 5) + 1].data = &(op[op_index]);\
	op_index++;\
	\
	lookup[(opcode_bits << 5) + (1 << 2) + 1].func = &address;\
	op[op_index].reg = REG_A;\
	op[op_index].RW = access;\
	op[op_index].addressing = ZEROPAGE;\
	op[op_index].opcode_func = &function;\
	lookup[(opcode_bits << 5) + 1].data = &(op[op_index]);\
	op_index++;\
	\
	lookup[(opcode_bits << 5) + (2 << 2) + 1].func = &address;\
	op[op_index].reg = REG_A;\
	op[op_index].RW = access;\
	op[op_index].addressing = IMMEDIATE;\
	op[op_index].opcode_func = &function;\
	lookup[(opcode_bits << 5) + 1].data = &(op[op_index]);\
	op_index++;\
	\
	lookup[(opcode_bits << 5) + (3 << 2) + 1].func = &address;\
	op[op_index].reg = REG_A;\
	op[op_index].RW = access;\
	op[op_index].addressing = ABSOLUTE;\
	op[op_index].opcode_func = &function;\
	lookup[(opcode_bits << 5) + 1].data = &(op[op_index]);\
	op_index++;\
	\
	lookup[(opcode_bits << 5) + (4 << 2) + 1].func = &address;\
	op[op_index].reg = REG_A;\
	op[op_index].RW = access;\
	op[op_index].addressing = INDIRECT_INDEXED;\
	op[op_index].opcode_func = &function;\
	lookup[(opcode_bits << 5) + 1].data = &(op[op_index]);\
	op_index++;\
	\
	lookup[(opcode_bits << 5) + (5 << 2) + 1].func = &address;\
	op[op_index].reg = REG_A;\
	op[op_index].RW = access;\
	op[op_index].addressing = ZEROPAGE_X_INDEXED;\
	op[op_index].opcode_func = &function;\
	lookup[(opcode_bits << 5) + 1].data = &(op[op_index]);\
	op_index++;\
	\
	lookup[(opcode_bits << 5) + (6 << 2) + 1].func = &address;\
	op[op_index].reg = REG_A;\
	op[op_index].RW = access;\
	op[op_index].addressing = ABSOLUTE_Y_INDEXED;\
	op[op_index].opcode_func = &function;\
	lookup[(opcode_bits << 5) + 1].data = &(op[op_index]);\
	op_index++;\
	\
	lookup[(opcode_bits << 5) + (7 << 2) + 1].func = &address;\
	op[op_index].reg = REG_A;\
	op[op_index].RW = access;\
	op[op_index].addressing = ABSOLUTE_X_INDEXED;\
	op[op_index].opcode_func = &function;\
	lookup[(opcode_bits << 5) + 1].data = &(op[op_index]);\
	op_index++;\




#define opcode2(opcode_bits,function,access,reg_stored)\
	lookup[(opcode_bits << 5) + 1].func = &address;\
	op[op_index].reg = reg_stored;\
	op[op_index].RW = access;\
	op[op_index].addressing = IMMEDIATE;\
	op[op_index].opcode_func = &function;\
	lookup[(opcode_bits << 5) + 1].data = &(op[op_index]);\
	op_index++;\
	\
	lookup[(opcode_bits << 5) + (1 << 2) + 1].func = &address;\
	op[op_index].reg = reg_stored;\
	op[op_index].RW = access;\
	op[op_index].addressing = ZEROPAGE;\
	op[op_index].opcode_func = &function;\
	lookup[(opcode_bits << 5) + 1].data = &(op[op_index]);\
	op_index++;\
	\
	lookup[(opcode_bits << 5) + (2 << 2) + 1].func = &address;\
	op[op_index].reg = reg_stored;\
	op[op_index].RW = access;\
	op[op_index].addressing = ACCUMULATOR;\
	op[op_index].opcode_func = &function;\
	lookup[(opcode_bits << 5) + 1].data = &(op[op_index]);\
	op_index++;\
	\
	lookup[(opcode_bits << 5) + (3 << 2) + 1].func = &address;\
	op[op_index].reg = reg_stored;\
	op[op_index].RW = access;\
	op[op_index].addressing = ABSOLUTE;\
	op[op_index].opcode_func = &function;\
	lookup[(opcode_bits << 5) + 1].data = &(op[op_index]);\
	op_index++;\
	\
	lookup[(opcode_bits << 5) + (5 << 2) + 1].func = &address;\
	op[op_index].reg = reg_stored;\
	op[op_index].RW = access;\
	op[op_index].addressing = ZEROPAGE_X_INDEXED;\
	op[op_index].opcode_func = &function;\
	lookup[(opcode_bits << 5) + 1].data = &(op[op_index]);\
	op_index++;\
	\
	lookup[(opcode_bits << 5) + (7 << 2) + 1].func = &address;\
	op[op_index].reg = reg_stored;\
	op[op_index].RW = access;\
	op[op_index].addressing = ABSOLUTE_X_INDEXED;\
	op[op_index].opcode_func = &function;\
	lookup[(opcode_bits << 5) + 1].data = &(op[op_index]);\
	op_index++;\








typedef void (*opcode_handler)(Cpu_6502*, void *);
typedef void (*opcode_runner)(uint8_t, uint8_t *, uint8_t *);


typedef struct 
{
	opcode_handler func;
	void * data;	
}
lookup_pair;


enum addressing_types
{
	ACCUMULATOR,
	IMMEDIATE,
	ABSOLUTE,
	ABSOLUTE_X_INDEXED,
	ABSOLUTE_Y_INDEXED,
	INDEXED_INDIRECT,
	INDIRECT_INDEXED,
	ZEROPAGE,
	ZEROPAGE_Y_INDEXED,
	ZEROPAGE_X_INDEXED
};



enum register_types
{
	REG_A,
	REG_Y,
	REG_X
};



enum read_rmw_write
{
	READ,
	WRITE,
	READ_MODIFY_WRITE
};


typedef struct
{
	enum register_types reg;
	enum addressing_types addressing;
	enum read_rmw_write RW;
	opcode_runner opcode_func;
}
Mem_op;

#include "opcodes.h"

lookup_pair lookup[0xff];
Mem_op op[0xff];
int op_index = 0;




/* setup opcodes to be decoded */

opcode1(0,Or,READ);
opcode1(1,And,READ);
opcode1(2,Eor,READ);
opcode1(3,AddCarry,READ);
opcode1(4,LoadStore,WRITE);
opcode1(5,Compare,READ);
opcode1(6,LoadStore,READ);
opcode1(7,Substract,READ);


opcode2(0,ArithmeticShiftLeft,READ_MODIFY_WRITE,NULL);
opcode2(1,RotateLeft,READ_MODIFY_WRITE,NULL);
opcode2(2,LogicalShiftRight,READ_MODIFY_WRITE,NULL);
opcode2(3,RotateRight,READ_MODIFY_WRITE,NULL);
opcode2(4,LoadStore,WRITE,REG_X);
opcode2(5,LoadStore,READ,REG_X);
opcode2(6,Decrement,READ_MODIFY_WRITE,NULL);
opcode2(7,Increment,READ_MODIFY_WRITE,NULL);




Cpu_6502 *init_Cpu()
{
	Cpu_6502 *cpu = (Cpu_6502*)calloc(sizeof(Cpu_6502));	
	ym3438_t *fm = 	(ym3438_t*)malloc(sizeof(ym3438_t));
	OPN2_Reset(fm);
	cpu->fm = fm;
	cpu->PC = 0x0200;
	cpu->S = 0x1FF;
	cpu->cycles = 0;
	return cpu;
}


/* one read and one write cycle */
void advance_Cpu(Cpu_6502 *cpu)
{
	*(lookup[(cpu->data)[PC]])(cpu);
}



