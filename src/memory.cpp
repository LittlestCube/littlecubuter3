#include <memory.h>
#include <main.h>

#include <fstream>

#include <termios.h>
#include <unistd.h>

#include <sys/stat.h>

Memory::Memory()
{
	mem = new u16[0x10000];
	memset(mem, 0, 0x10000);
	
	mem[KBSR] = 0x0000;
	mem[KBDR] = 0xFFFF;
	mem[DSR] = 0x8000;
	mem[DDR] = 0x0000;
	mem[MCR] = 0x8000;
	
	romOrig = 0;
}

bool Memory::loadROM(const char* path)
{
	ifstream romfile(path, ios::in | ios::binary);
	
	if (!romfile.good())
	{
		cout << endl << "ROM not found at path " << path << endl;
		
		return false;
	}
	
	struct stat sb{};
	stat(path, &sb);
	int romLen = sb.st_size;
	
	char* rom = new char[romLen];
	
	romfile.read(rom, romLen);
	
	u16 orig = (rom[0] << 8) | rom[1];
	rom = &rom[2];
	
	for (int i = 0; i < romLen; i++)
	{
		mem[i + orig] = (static_cast<u8>(rom[i * 2]) << 8) | static_cast<u8>(rom[(i * 2) + 1]);
	}
	
	cpu.pc = orig;
	
	return true;
}

u16 Memory::readMem(u16 addr)
{
	switch (addr)
	{
		case KBDR:
		{
			mem[KBSR] = 0x0000;
			
			cpu.intReady = true;
			
			break;
		}
	}
	
	return mem[addr];
}

void Memory::writeMem(u16 addr, u16 value)
{
	switch (addr)
	{
		case 0xFFFE:
		{
			if ((value & 0x8000) == 0x0000)
			{
				if (inOS)
				{
					inOS = false;
					
					cpu.pc = romPC;
					cpu.privilege = 1;
				}
				
				else
				{
					quit = true;
				}
			}
			
			break;
		}
	}
	
	mem[addr] = value;
}