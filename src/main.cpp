#include <thread>

#include <sys/ioctl.h>

#include <termios.h>
#include <unistd.h>

#include <main.h>

using namespace std;

CPU cpu;
Memory mem;

u16 romPC;

bool inOS;
bool quit;

char c;

int main(int argc, char** argv)
{
	cpu = CPU();
	mem = Memory();
	
	quit = false;
	
	if (argc > 1)
	{
		mem.loadROM(argv[1]);
	}
	
	else
	{
		printf("usage: lc3 [rom path]\n");
		
		return 1;
	}
	
	romPC = cpu.pc;
	
	if (mem.loadROM("./lc3os.bin"))
	{
		inOS = true;
		
		cpu.pc = 0x200;
	}
	
	static struct termios revert;
	static struct termios newt;
	
	tcgetattr(0, &revert);
	newt = revert;
	
	newt.c_lflag &= ~ICANON;
	newt.c_lflag &= ~ECHO;
	tcsetattr(0, TCSANOW, &newt);
	setbuf(stdin, NULL);
	
	while (!quit)
	{
		if (kbhit())
		{
			c = cin.get();
			
			mem.mem[Memory::KBSR] = 0x8000;
			mem.mem[Memory::KBDR] = (u16) c;
		}
		
		cpu.cycle();
		
		if (mem.mem[Memory::DSR] >> 15 && mem.mem[Memory::DDR] != 0x0000)
		{
			putchar((char) mem.mem[Memory::DDR]);
			cout << flush;
			
			mem.mem[Memory::DDR] = 0x0000;
		}
	}
	
	tcsetattr(0, TCSADRAIN, &revert);
	
	cout << endl;
}

int kbhit()
{
	int bytesWaiting;
	ioctl(0, FIONREAD, &bytesWaiting);
	return bytesWaiting;
}