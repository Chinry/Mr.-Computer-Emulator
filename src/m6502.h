#include <stdint.h>
#include "../lib/ym3436.h"
typedef struct {
	uint8_t A;
	uint8_t X;
	uint8_t Y;
	uint8_t P;
	uint16_t PC; //not true location all of the time
	uint16_t S;
	ym3438_t *fm;
	uint8_t cycles;
	uint8_t data[0xffff];
} 
Cpu_6502;


Cpu_6502 *init_Cpu();
void advance_Cpu(Cpu_6502*);
void advance_Fm(Cpu_6502*, uint32_t *buffer);
void load_data(Cpu_6502*, FILE*); 

