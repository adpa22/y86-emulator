#ifndef _Y86EMUL_H_
#define _Y86EMUL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>


//making my own boolean since C doesnt have a boolean
typedef enum {
	false,true
}boolean;


//to put a long into char to put into mem
union holdLong{
	long num;
	unsigned char byte[4];
}longh;


//makes it easier to store bytes using union
union Byte{
	struct pair{ //upperhalf and lower half
		unsigned int upper:4;
		unsigned int lower:4;
	}half;
	unsigned char overbyte;
}storeByte;

void runProg(); //gets the instructions and executes them

#endif //_Y86EMUL_H_


