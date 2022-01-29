#pragma once

#include <common.h>

class Memory
{
public:
	static const u16 KBSR = 0xFE00;
	static const u16 KBDR = 0xFE02;
	static const u16 DSR = 0xFE04;
	static const u16 DDR = 0xFE06;
	static const u16 MCR = 0xFFFE;
	
	Memory();
	
	bool loadROM(const char* path);
	
	u16* mem;
	
	u16 romOrig;
	
	u16 readMem(u16 addr);
	void writeMem(u16 addr, u16 value);
};