//
// Dynamic 2D array memory allocation in C
//  2015.3.29 by Pierre Chen
// MIAT lab, National Central University
//

#ifndef _ARRAY2D_H
#define _ARRAY2D_H

unsigned char** UC2D(int nr, int nc);
void freeUC2D(unsigned char** myArray, int nr);
int** INT2D(int nr, int nc);
void freeINT2D(int** myArray, int nr);
float** FLOAT2D(int nr, int nc);
void freeFLOAT2D(float** myArray, int nr);

#endif

