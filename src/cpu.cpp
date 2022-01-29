#include <cpu.h>

#include <main.h>

#define dr r[((instr >> 9) & 7)]
#define sr1 r[((instr >> 6) & 7)]

CPU::CPU()
{
	r = new e16[8];
	memset(r, 0, 8);
	
	pc = 0;
	
	ssp = 0x2FFF;
	usp = 0x2FFF;
	
	psr = 2;
}

void CPU::cycle()
{
	u16 instr = (u16) mem.readMem(pc);
	
	/*printf("N: %d  Z: %d  P: %d  R0: %04X  R1: %04X  DDR: %04X  PC: %04X  INSTR: %04X  (", n, z, p, r[0], r[1], mem.mem[0xFE06], pc, instr);
	
	for (int i = 0; i < 0x1FFFFF; i++)
	{
		printf("");
		cout << flush;
	}*/
	
	switch ((instr >> 12) & 0xF)
	{
		case 0b0001: // ADD
		{
			//cout << "ADD)" << endl;
			
			if ((instr >> 5) & 1)
			{
				dr = sr1 + signExtend(instr & 0x1F, 5);
			}
			
			else
			{
				dr = sr1 + r[(instr & 7)];
			}
			
			updateCC(dr);
			
			pc += 1;
			break;
		}
		
		case 0b0101: // AND
		{
			//cout << "AND)" << endl;
			
			if ((instr >> 5) & 1)
			{
				dr = sr1 & signExtend(instr & 0x1F, 5);
			}
			
			else
			{
				dr = sr1 & r[(instr & 7)];
			}
			
			updateCC(dr);
			
			pc += 1;
			break;
		}
		
		case 0b0000: // BR
		{
			//cout << "BR)" << endl;
			
			pc += 1;
			
			if (((instr >> 9) & 7) & ((n << 2) | (z << 1) | p))
			{
				pc += signExtend(instr & 0x1FF, 9);
			}
			
			break;
		}
		
		case 0b1100: // JMP or RET
		{
			//cout << "JMP or RET)" << endl;
			
			pc = sr1;
			
			break;
		}
		
		case 0b0100: // JSR or JSRR
		{
			//cout << "JSR or JSRR)" << endl;
			
			pc += 1;
			
			r[7] = pc;
			
			if ((instr >> 11) & 1)
			{
				pc += signExtend(instr & 0x7FF, 11);
			}
			
			else
			{
				pc = sr1;
			}
			
			break;
		}
		
		case 0b0010: // LD
		{
			//cout << "LD)" << endl;
			
			pc += 1;
			
			dr = mem.readMem(pc + signExtend(instr & 0x1FF, 9));
			
			updateCC(dr);
			
			break;
		}
		
		case 0b1010: // LDI
		{
			//cout << "LDI)" << endl;
			
			pc += 1;
			
			dr = mem.readMem(mem.readMem(pc + signExtend(instr & 0x1FF, 9)));
			
			updateCC(dr);
			
			break;
		}
		
		case 0b0110: // LDR
		{
			//cout << "LDR)" << endl;
			
			pc += 1;
			
			dr = mem.readMem(sr1 + signExtend(instr & 0x3F, 6));
			
			updateCC(dr);
			
			break;
		}
		
		case 0b1110: // LEA
		{
			//cout << "LEA)" << endl;
			
			pc += 1;
			
			dr = pc + signExtend(instr & 0x1FF, 9);
			
			updateCC(dr);
			
			break;
		}
		
		case 0b1001: // NOT
		{
			//cout << "NOT)" << endl;
			
			pc += 1;
			
			dr = ~sr1;
			
			break;
		}
		
		case 0b1000: // RTI
		{
			//cout << "RTI)" << endl;
			
			if (privilege) // if user privilege
			{
				exception(0);
			}
			
			else // else admin privilege
			{
				pc = pop();
				psr = pop();
				
				userMode();
			}
			
			break;
		}
		
		case 0b0011: // ST
		{
			//cout << "ST)" << endl;
			
			pc += 1;
			
			mem.writeMem(pc + signExtend(instr & 0x1FF, 9), dr);
			
			break;
		}
		
		case 0b1011: // STI
		{
			//cout << "STI)" << endl;
			
			pc += 1;
			
			mem.writeMem(mem.readMem(pc + signExtend(instr & 0x1FF, 9)), dr);
			
			break;
		}
		
		case 0b0111: // STR
		{
			//cout << "STR)" << endl;
			
			pc += 1;
			
			mem.writeMem(((instr >> 6) & 7) + signExtend(instr & 0x3F, 6), dr);
			
			break;
		}
		
		case 0b1111: // TRAP
		{
			//cout << "TRAP)" << endl;
			
			pc += 1;
			
			r[7] = pc;
			
			pc = mem.readMem(instr & 0xFF);
			
			break;
		}
		
		default:
		{
			exception(1);
			
			break;
		}
	}
	
	if (mem.mem[Memory::KBSR] >> 15)
	{
		interrupt(0x80);
		
		mem.writeMem(Memory::KBSR, 0);
	}
}

inline e16 CPU::signExtend(u16 value, int size)
{
	int shift = 16 - size;
	
	int temp = (e16) (value << shift);
	return temp >> shift;
}

inline void CPU::updateCC(e16 result)
{
	psr &= 0xFFF8;
	
	if (result > 0)
	{
		p = 1;
	}
	
	else if (result < 0)
	{
		n = 1;
	}
	
	else
	{
		z = 1;
	}
}

inline void CPU::interrupt(u8 vector)
{
	u16 tempPSR = psr;
	
	supervisorMode();
	
	push(tempPSR);
	push(pc);
	
	priority = 4;
	
	pc = mem.readMem(vector | 0x100);
}

inline void CPU::exception(u8 vector)
{
	u16 tempPSR = psr;
	
	supervisorMode();
	
	push(tempPSR);
	push(pc);
	
	pc = mem.readMem(vector | 0x100);
}

inline void CPU::supervisorMode()
{
	if (privilege)
	{
		privilege = 0;
		
		usp = r[6];
		r[6] = ssp;
	}
}

inline void CPU::userMode()
{
	if (!privilege)
	{
		privilege = 1;
		
		ssp = r[6];
		r[6] = usp;
	}
}

inline void CPU::push(e16 value)
{
	r[6] -= 1;
	
	mem.writeMem((u16) r[6], value);
}

inline e16 CPU::pop()
{
	e16 value = mem.readMem((u16) r[6]);
	
	r[6] += 1;
	
	return value;
}