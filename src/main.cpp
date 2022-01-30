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
	
	tcgetattr(0, &revert);
	
	while (!quit)
	{
		cin.sync();
		
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
	static const int STDIN = 0;
	static bool initialized = false;
	
	if (!initialized)
	{
		// Use termios to turn off line buffering
		termios term;
		tcgetattr(STDIN, &term);
		term.c_lflag &= ~ICANON;
		tcsetattr(STDIN, TCSANOW, &term);
		setbuf(stdin, NULL);
		initialized = true;
	}
	
	int bytesWaiting;
	ioctl(STDIN, FIONREAD, &bytesWaiting);
	return bytesWaiting;
}