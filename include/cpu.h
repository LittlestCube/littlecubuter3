#pragma once

#include <common.h>

class CPU
{
public:
	CPU();
	
	e16* r;
	
	u16 pc;
	
	u16 ssp;
	u16 usp;
	
	union
	{
		struct
		{
			union
			{
				struct
				{
					bool n : 1;
					bool z : 1;
					bool p : 1;
				};
				
				u8 cc : 3;
			};
			
			u8 : 5;
			u8 priority : 3;
			u8 : 4;
			bool privilege : 1;
		};
		
		u16 psr;
	};
	
	void cycle();
	
	e16 signExtend(u16 value, int size);
	
	void updateCC(e16 result);
	
	void interrupt(u8 vector);
	void exception(u8 vector);
	
	void supervisorMode();
	void userMode();
	
	void push(e16 value);
	e16 pop();
};