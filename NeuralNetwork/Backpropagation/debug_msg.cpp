
extern "C"{
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
}
#include "debug_msg.h"

void delay(int millisecond){
	Sleep(millisecond);
}

void clear_screen(){
	system("cls");
}

void pause(){
	printf("press any key to start...\n");
	fgetc(stdin);
}

void error_handle(){
	DEBUG("error_handle: error occurring.");
	printf("press any key to close...\n");
	fgetc(stdin);
	exit(1);
}
