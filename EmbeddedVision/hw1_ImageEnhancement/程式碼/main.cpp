//
// image enhancement
//
#include <iostream>
#include "stdlib.h"
#include "bmp.h"
#define M 256 
#define WIN_SIZE 9
#define WIN_SIZE_2 (WIN_SIZE >> 1) 

using namespace std;

//global variable
int bin[M];
unsigned char **ima, **bima;
int nr, nc;//image height and width
int fi, i, j;
int index, N;

char filename[128];
bool isvalid;

//state
int x0 = 1, x1, x2, x3, x4;
int x20 = 1,x21 ,x22 ,x23 ,x24 ,x25 ,x26 ,x27 ,x28 ,x29 ,x210 ,x211, x212;

void grafcet0();
void action0();
void grafcet2_ADPHE_SHE();
void action2();

int main(int argc, char** argv) {
	
	while(x4 != 1) grafcet0();
	
	system("PAUSE");
	return 1;
}

void grafcet0(){
	
	action0();
	if(x0 == 1){ x0 = 0; x1 = 1;}
	else if(x1 == 1){ x1 = 0; x2 = 1;}
	else if(x2 == 1 && x212 == 1){ x2 = 0; x3 = 1;}
	else if(x3 == 1){ x3 = 0; x4 = 1;}
	else if(x4 == 1){ x4 = 0; x0 = 1;} //null state
}

void action0(){
	if(x0 == 1){
		isvalid = false;
	    //read bmp image from file
	    cout << "Enter input filename:";
		cin >> filename;
	}
	else if(x1 == 1){
		isvalid = Read_BMP(filename, ima, nr, nc);
		if (!isvalid) exit(1);
		bima=UC2D(nr, nc);
		Write_BMP_8bits("ima.bmp", ima, nr, nc);
	}
	else if(x2 == 1){ grafcet2_ADPHE_SHE(); }
	else if(x3 == 1){ Write_BMP_8bits("ADPHE_SHE.bmp", bima, nr, nc); }
}

void grafcet2_ADPHE_SHE(){
	action2();
	if(x20 == 1){x20 = 0; x21 = 1; x22 = 1;}
	else if(x21 == 1 && x22 == 1 && i == nr){x21 = 0; x22 = 0; x23 = 1;}
	else if(x23 == 1){x23 = 0; x24 = 1; x25 = 1; x26 = 1; x27 = 1;}
	else if(x24 == 1 && x25 == 1 && x26 == 1 && x27 == 1 && fi == M){x24 = 0; x25 = 0; x26 = 0; x27 = 0; x28 = 1;}
	else if(x28 == 1){x28 = 0; x29 = 1; x210 = 1;}
	else if(x29 == 1 && x210 == 1 && index == M){x29 = 0; x210 = 0; x211 = 1;}
	else if(x211 == 1 && i == nr){x211 = 0; x212 = 1;}
	else if(x212 == 1){x212 = 0; x20 = 1;}
}

void action2(){
	static int first_one, last_two, last_one, head, nz_cnt, max_sum, max_cnt, bi;
	static int Tup, Tdown;
	static int buf[WIN_SIZE];
	int z, temp;
	int pixel;
	
	if(x20 == 1){ i = j = 0; for(int index = 0;index < M;index++) bin[index] = 0;}
	if(x21 == 1){ pixel = ima[i][j]; if(j == nc - 1) i++, j=0; else j++;}
	if(x22 == 1){ bin[pixel]++;}
	if(x23 == 1){ 
		for(int z = 0;z < WIN_SIZE;z++) buf[z] = 0;
		first_one = last_two = last_one = -1; head = fi = nz_cnt = max_sum = max_cnt = 0; 
		bi = M - 1;
	}
	if(x24 == 1){
		if(bin[bi] && last_one < 0) last_one = bi;
		else if(bin[bi] && last_two < 0) last_two = bi;
		bi--;
	}
	if(x25 == 1){
		if(bin[fi] && first_one < 0) first_one = fi;
	}
	if(x26 == 1){
		buf[head] = bin[fi];
		//lmax
		int max = buf[0];
		for(int z = 1;z < WIN_SIZE;z++)
			if(max < buf[z]) max = buf[z];
		max_sum += ((buf[WIN_SIZE_2] >= max) ? buf[WIN_SIZE_2] : 0);
		max_cnt += ((buf[WIN_SIZE_2] >= max) ? 1 : 0);
		
		head = (head + 1) % WIN_SIZE;
	}
	if(x27 == 1){
		if(bin[fi++]) nz_cnt++;
	}
	if(x28 == 1){
		Tup = max_sum/(max_cnt+1);
		int temp1, temp2;
		temp1 = nr * nc; temp2 = Tup * nz_cnt;
		Tdown = ((temp1 > temp2) ? temp2 : temp1) / (M << 1);
		index = 1; N = i = j = 0;
		bin[first_one] = 0; bin[last_one] = bin[last_two];
	}
	if(x29 == 1){
		z = index; temp = bin[z];
		if(bin[z] >= Tup) temp = Tup;
		else if(bin[z] == 0) temp = 0;
		else if(bin[z] <= Tdown) temp = Tdown;
	}
	if(x210 == 1){
		bin[z] = temp + bin[z - 1];
		N += temp;
		index++;
	}
	if(x211 == 1){
		bima[i][j] = bin[ima[i][j]] * (M - 1) / (N);
		if(j == nc - 1){ i++, j = 0;}
		else j++;
	}
	if(x212 == 1){ /*no operation*/ }
	
}

