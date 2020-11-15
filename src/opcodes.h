
#define ZERO_FLAG (1 << 1)
#define CARRY_FLAG 1
#define INTERRUPT_DISABLE_FLAG (1 << 2)
#define DECIMAL_FLAG (1 << 3)
#define OVERFLOW_FLAG (1 << 6)
#define NEGATIVE_FLAG (1 << 7)



void Branch(Cpu_6502 *cpu, void *comparison)
{
	
	num = ((PC & 0x00FF) + (signed int8_t)((cpu->data)[PC + 1]) ) ? 4 : 3;

	if (*((char*)comparison)) 
	{
		if (cpu->cycle < num) {(cpu->cycle)++;}
		else {PC += (signed int8_t)(data[PC]); cpu->cycle = 0;}
	}
	else
	{
		if (cpu->cycle < 1) {(cpu->cycle)++;}
		else {PC+=2; cpu->cycle = 0;}
	}
}

void JSR(Cpu_6502 *cpu)
{
	if (cpu->cycle < 4) {(cpu->cycle)++;}
	else
	{
		(cpu->data)[cpu->S] = (uint8_t)((cpu->PC + 1) >> 8):
		(cpu->data)[cpu->S - 1] = (uint8_t)((cpu->PC + 1) & 0x00FF):
		cpu->sp -= 2;
	}
}



void RTI(Cpu_6502 *cpu, void *unused)
{
	if (cpu->cycle < 5) {(cpu->cycle)++;}
	else
	{
		cpu->P = (cpu->data)[S];
		cpu->PC = 0;
		cpu->PC += (cpu->data)[S + 1];
		cpu->PC += ((cpu->data)[S + 2] << 8);
		(cpu->S) += 3;
	}
}


void RTS(Cpu_6502 *cpu, void *unused)
{
	if (cpu->cycle < 5) {(cpu->cycle)++;}
	else
	{
		cpu->PC = 0;
		cpu->PC += (cpu->data)[S];
		cpu->PC += ((cpu->data)[S + 1] << 8);
		(cpu->PC)++;
		(cpu->S) += 2;
	}
}


int calcCycles(Cpu_6502 *cpu, Mem_op *data)
{
	enum addressing_types addressing = (*((Mem_op*)data)).addressing;
	uint8_t RW = (*((Mem_op*)data)).RW;
	if (RW == READ)
	{
		switch(addressing)
		{
			case IMMEDIATE:
			case ACCUMULATOR:
				return 1;
			case ABSOLUTE:
			case ZEROPAGE_Y_INDEXED:
			case ZEROPAGE_X_INDEXED:
				return 3;
			case ZEROPAGE:
				return 2;
			case INDEXED_INDIRECT:
				return 5;
			case ABSOLUTE_X_INDEXED:
				uint8_t low = (cpu->data)[PC + 1];
				uint8_t high = (cpu->data)[PC + 2];
				uint16_t address = low + (high << 8);
				return = ((address & 0x00FF) + cpu->X ) ? 4 : 3;
			case ABSOLUTE_Y_INDEXED:
				uint8_t low = (cpu->data)[PC + 1];
				uint8_t high = (cpu->data)[PC + 2];
				uint16_t address = low + (high << 8);
				return ((address & 0x00FF) + cpu->Y ) ? 4 : 3;
			case INDIRECT_INDEXED:
				uint8_t low = (cpu->data)[(cpu->data)[PC + 1]];
				uint8_t high = (cpu->data)[(cpu->data)[PC + 1] + 1];
				uint16_t address = low + (high << 8);
				return ((address & 0x00FF) + cpu->Y ) ? 5 : 4;
		}
	}
	else if (RW == READ_MODIFY_WRITE)
	{
	
		switch(addressing)
		{
			case ABSOLUTE:
			case ZEROPAGE_Y_INDEXED:
			case ZEROPAGE_X_INDEXED:
				return 4;
			case ZEROPAGE:
				return 3;
			case ABSOLUTE_X_INDEXED:
			case ABSOLUTE_Y_INDEXED:
				return 5;			
		}

	}
	else
	{
		switch(addressing)
		{
			case ZEROPAGE_Y_INDEXED:
			case ZEROPAGE_X_INDEXED:
			case ABSOLUTE:
				return 2;
			case ZEROPAGE:
				return 1;
			case INDEXED_INDIRECT:
			case INDIRECT_INDEXED:
				return 4;
			case ABSOLUTE_Y_INDEXED:
			case ABSOLUTE_X_INDEXED:
				return 3;
		}
	}
	return 0;

}




void address(Cpu *cpu, void *data)
{
	enum register_types reg = (*((Mem_op*)data)).reg;
	enum addressing_types addressing = (*((Mem_op*)data)).addressing;
	uint8_t RW = (*((Mem_op*)data)).RW;
	opcode_runner func = (*((Mem_op*)data)).opcode_func;
	int wait = calcCycles(cpu, (Mem_op*)data);
	if (cpu->cycle < wait) {(cpu->cycle)++;}
	else
	{
		uint8_t *reg_used;	
		switch(reg)
		{
			case REG_A:
				reg_used = &(cpu->A);
				break;
			case REG_X:
				reg_used = &(cpu->X);
				break;
			case REG_Y:
				reg_used = &(cpu->Y);
		}
		switch(addressing)
		{
			case ACCUMULATOR:
				(*func)(*reg_used, reg_used, &(cpu->P));
				cpu->PC += 1
			case IMMEDIATE:
				(*func)((cpu->data)[PC + 1], reg_used, &(cpu->P));
				cpu->PC += 2
				break;
			case ABSOLUTE:
				uint8_t low = (cpu->data)[PC + 1];
				uint8_t high = (cpu->data)[PC + 2]:
				uint16_t address = low + (high << 8);
				if (RW == READ)
					(*func)((cpu->data)[address], reg_used, &(cpu->P));
				else if (RW == READ_MODIFY_WRITE)
					(*func)((cpu->data)[address], &((cpu->data)[address]), &(cpu->P));
				else
					(*func)(*reg_used, &((cpu->data)[address]), &(cpu->P));
				cpu->PC += 3;	
				break;
			case ZEROPAGE_Y_INDEXED:
				uint8_t address = (cpu->data)[PC + 1] + cpu->Y;
				if (RW == READ)
					(*func)((cpu->data)[address], reg_used, &(cpu->P));
				else if (RW == READ_MODIFY_WRITE)
					(*func)((cpu->data)[address], &((cpu->data)[address]), &(cpu->P));
				else
					(*func)(*reg_used, &((cpu->data)[address]), &(cpu->P));
				cpu->PC += 2;	
				break;
			case ZEROPAGE_X_INDEXED:
				uint8_t address = (cpu->data)[PC + 1] + cpu->X;
				if (RW == READ)
					(*func)((cpu->data)[address], reg_used, &(cpu->P));
				else if (RW == READ_MODIFY_WRITE)
					(*func)((cpu->data)[address], &((cpu->data)[address]), &(cpu->P));
				else
					(*func)(*reg_used, &((cpu->data)[address]), &(cpu->P));
				cpu->PC += 2;	
				break;
			case ZEROPAGE:
				uint8_t address = (cpu->data)[PC + 1];
				if (RW == READ)
					(*func)((cpu->data)[address], reg_used, &(cpu->P));
				else if (RW == READ_MODIFY_WRITE)
					(*func)((cpu->data)[address], &((cpu->data)[address]), &(cpu->P));
				else
					(*func)(*reg_used, &((cpu->data)[address]));
				cpu->PC += 2;	
				break;
			case ABSOLUTE_X_INDEXED:
				uint8_t low = (cpu->data)[PC + 1];
				uint8_t high = (cpu->data)[PC + 2]:
				uint16_t address = low + (high << 8);
				if (RW == READ)
					(*func)((cpu->data)[address], reg_used, &(cpu->P));
				else if (RW == READ_MODIFY_WRITE)
					(*func)((cpu->data)[address], &((cpu->data)[address]), &(cpu->P));
				else
					(*func)(*reg_used, &((cpu->data)[address]), &(cpu->P));
				cpu->PC += 3;	
				break;
			case ABSOLUTE_Y_INDEXED:
				uint8_t low = (cpu->data)[PC + 1];
				uint8_t high = (cpu->data)[PC + 2]:
				uint16_t address = low + (high << 8);
				if (RW == READ)
					(*func)((cpu->data)[address], reg_used, &(cpu->P));
				else if (RW == READ_MODIFY_WRITE)
					(*func)((cpu->data)[address], &((cpu->data)[address]), &(cpu->P));
				else
					(*func)(*reg_used, &((cpu->data)[address]), &(cpu->P));
				cpu->PC += 3;	
				break;
			case INDEXED_INDIRECT:
				uint8_t low = (cpu->data)[(cpu->data)[PC + 1] + cpu->X];
				uint8_t high = (cpu->data)[(cpu->data)[PC + 1] + 1 + cpu->X];
				uint16_t address = low + (high << 8);
				if (RW == READ)
					(*func)((cpu->data)[address], reg_used, &(cpu->P));
				else
					(*func)(*reg_used, &((cpu->data)[address]), &(cpu->P));
				cpu->PC += 2;
				break;
			case INDIRECT_INDEXED:
				uint8_t low = (cpu->data)[(cpu->data)[PC + 1]];
				uint8_t high = (cpu->data)[(cpu->data)[PC + 1] + 1];
				uint16_t address = low + (high << 8);
				if (RW == READ)
					(*func)((cpu->data)[address + cpu->Y], reg_used, &(cpu->P));
				else
					(*func)(*reg_used, &((cpu->data)[address + cpu->Y]), &(cpu->P));
				cpu->PC += 2;
		}
		cpu->cycles = 0;


	}

}

void LoadStore(uint8_t source, uint8_t * destination, uint8_t *P){
	*destination = source;
}
void And(uint8_t source, uint8_t *destination, uint8_t *P){
	*destination &= source;
	if (*destination) *P &= ~(ZERO_FLAG);
	else *P |= ZERO_FLAG;
	if (*destination >= 0x80) *P |= NEGATIVE_FLAG;
	else *P &= ~(NEGATIVE_FLAG);
}
void Compare(uint8_t source, uint8_t *destination, uint8_t *P){
	if (source == *destination) *P |= ZERO_FLAG;
	else *P &= ~(ZERO_FLAG);
	if ((*destination - source) >= 0x80) *P |= NEGATIVE_FLAG;
	else *P &= ~(NEGATIVE_FLAG);
	if (*destination >= source) *P |= CARRY_FLAG;
	else *P &= ~CARRY_FLAG;
}
void Subtract(uint8_t source, uint8_t *destination, uint8_t *P)
{
	Compare(source, destination, P);
	signed int result = (signed int)(*destination) - (signed int)source - (signed int)(*P & CARRY_FLAG);
	if(result > 127 || result < -128) *P |= OVERFLOW_FLAG;
	else *P &= ~(OVERFLOW_FLAG);
	*destination -= source - (*P & CARRY_FLAG);
}
void Increment(uint8_t source, uint8_t *destination, uint8_t *P)
{
	(*destination)++;
	if (*destination) *P &= ~(ZERO_FLAG);
	else *P |= ZERO_FLAG;
	if (*destination >= 0x80) *P |= NEGATIVE_FLAG;
	else *P &= ~(NEGATIVE_FLAG);
}
void Decrement(uint8_t source, uint8_t *destination, uint8_t *P)
{
	(*destination)--;
	if (*destination) *P &= ~(ZERO_FLAG);
	else *P |= ZERO_FLAG;
	if (*destination >= 0x80) *P |= NEGATIVE_FLAG;
	else *P &= ~(NEGATIVE_FLAG);
}
void RotateRight(uint8_t source, uint8_t *destination, uint8_t *P)
{
	uint8_t carry = (*P & CARRY_FLAG) << 7;
	*P =  (*P & ~(CARRY_FLAG)) | (CARRY_FLAG & *destination);
	*destination = (*destination >> 1) + carry;
	if (*destination) *P &= ~(ZERO_FLAG);
	else *P |= ZERO_FLAG;
	if (*destination >= 0x80) *P |= NEGATIVE_FLAG;
	else *P &= ~(NEGATIVE_FLAG);
}
void RotateLeft(uint8_t source, uint8_t *destination, uint8_t *P)
{
	uint8_t carry = *P & CARRY_FLAG;
	*P =  (*P & ~(CARRY_FLAG)) | (*destination >> 7);
	*destination = (*destination << 1) + carry;
	if (*destination) *P &= ~(ZERO_FLAG);
	else *P |= ZERO_FLAG;
	if (*destination >= 0x80) *P |= NEGATIVE_FLAG;
	else *P &= ~(NEGATIVE_FLAG);
}
void ArithmeticShiftLeft(uint8_t source, uint8_t *destination, uint8_t *P)
{
	*P =  (*P & ~(CARRY_FLAG)) | (source >> 7);
	*destination = *destination << 1;
	if (*destination) *P &= ~(ZERO_FLAG);
	else *P |= ZERO_FLAG;
	if (*destination >= 0x80) *P |= NEGATIVE_FLAG;
	else *P &= ~(NEGATIVE_FLAG);
}
void LogicalShiftRight(uint8_t source, uint8_t *destination, uint8_t *P)
{
	*P =  (*P & ~(CARRY_FLAG)) | (CARRY_FLAG & source);
	*destination = *destination >> 1;
	if (*destination) *P &= ~(ZERO_FLAG);
	else *P |= ZERO_FLAG;
	if (*destination >= 0x80) *P |= NEGATIVE_FLAG;
	else *P &= ~(NEGATIVE_FLAG);
}
void Eor(uint8_t source, uint8_t *destination, uint8_t *P)
{
	*destination ^= source;
	if (*destination) *P &= ~(ZERO_FLAG);
	else *P |= ZERO_FLAG;
	if (*destination >= 0x80) *P |= NEGATIVE_FLAG;
	else *P &= ~(NEGATIVE_FLAG);
}
void Or(uint8_t source, uint8_t *destination, uint8_t *P)
{
	
	*destination |= source;
	if (*destination) *P &= ~(ZERO_FLAG);
	else *P |= ZERO_FLAG;
	if (*destination >= 0x80) *P |= NEGATIVE_FLAG;
	else *P &= ~(NEGATIVE_FLAG);
}

void AddCarry(uint8_t source, uint8_t *destination, uint8_t *P)
{
	uint8_t prev_carry = *P & CARRY_FLAG;
	if (((uint16_t)source + *destination + prev_carry) > 0xff) *P |= CARRY_FLAG;
	else *P &= ~(CARRY_FLAG);
	uint8_t result = source + *destination + prev_carry; 
	if ((*destination ^ result) & (source ^ result) & 0x80) *P |= OVERFLOW_FLAG;
	else *P &= ~(OVERFLOW_FLAG);
	*destination = *destination + source;
	if (*destination) *P &= ~(ZERO_FLAG);
	else *P |= ZERO_FLAG;
	if (*destination >= 0x80) *P |= NEGATIVE_FLAG;
	else *P &= ~(NEGATIVE_FLAG);
}


