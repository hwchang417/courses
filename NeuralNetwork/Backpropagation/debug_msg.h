
#ifndef DEBUG_MSG_H
#define DEBUG_MSG_H

#include <stdio.h>

//#define _DEBUG

#ifdef _DEBUG
#define DEBUG(format, args...) printf("[%s:%d] "format, __FILE__, __LINE__, ##args)
#else
#define DEBUG(args...)
#endif

#define _DEBUG2

#ifdef _DEBUG2
#define DEBUG2(format, args...) printf(format, ##args)
#else
#define DEBUG2(args...)
#endif

void delay(int millisecond);
void clear_screen();
void pause();
void error_handle();

#endif
