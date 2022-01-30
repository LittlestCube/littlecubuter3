#pragma once

#include <iostream>

using namespace std;

typedef uint8_t u8;
typedef uint16_t u16;

typedef signed char e8;
typedef signed short e16;

extern u16 romPC;

extern bool inOS;
extern bool quit;

extern char c;

extern void getonech();