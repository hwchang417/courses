//
// image enhancement
//  2015.4.20 by Pierre Chen
// MIAT lab, National Central University
//
#include <iostream>
#include <cstdio>
#include <ctime>
#include "stdlib.h"
#include "string.h"
#include "bmp.h"
#define GRAY_LEVEL 256
//test snippet
#include <windows.h>
#define MAX_FILE 100
#define MAX_FILE_NAME 256

void HE(unsigned char **ima, unsigned char **bima, int nr,int nc);
void AHE(unsigned char **ima, unsigned char **bima, int nr,int nc);
void CLAHE(unsigned char **ima, unsigned char **bima, int nr,int nc, int ratio);
void ACEBSF(unsigned char **ima, unsigned char **bima, int nr,int nc);
void DSHE(unsigned char **ima, unsigned char **bima, int nr, int nc, const float C);
void SHE(unsigned char **ima, unsigned char **bima, int nr, int nc);
void DPHE(unsigned char **ima, unsigned char **bima, int nr, int nc);
void ADPHE_SHE(unsigned char **ima, unsigned char **bima, int nr, int nc);

unsigned char MSF(unsigned char f);

using namespace std;

int FindFile(char *file, char arr[MAX_FILE][MAX_FILE_NAME]){
	char* nfilen;
	unsigned f;
    char desOfile[80]; //destination of file, load to a LPDIRECT3DTEXTURE9
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile((LPCSTR)file, &findFileData);
    std::cout <<"Picture files in dir:" <<"\n"<<file<<"\n\n"<<std::endl;
    std::cout <<findFileData.cFileName<<"\n"<<std::endl;
    nfilen = findFileData.cFileName;
    strcpy(desOfile,"C:\\Users\\MIAT\\Desktop\\MIAT\\1051master\\Embedded_vision\\HW1\\DevC++\\");
    strcat(desOfile,nfilen);
 
//change to something to navigate between images, add bmp for search 
    for (f=0;f<MAX_FILE;f++) //change 
    {
        //tomalloc    
        if(!FindNextFile(hFind,&findFileData)) break;
        char *temp = findFileData.cFileName;
        strcpy(arr[f], temp);
        strcpy(desOfile,"C:\\Users\\MIAT\\Desktop\\MIAT\\1051master\\Embedded_vision\\HW1\\DevC++\\");
        strcat(desOfile,nfilen);
        std::cout<<arr[f]<<std::endl;
        //std::cout<<desOfile<<"\n\n"<<std::endl;
          /*arr[f]=desOfile;  for actual app IMPORTANT*/
 
    } 
    
	return f;
}

 

int main(int argc, char** argv) {
	
	unsigned char **ima, **bima;
	int nr,nc; //image height and width
	char ofilename[128];
	bool isvalid = false;
	
	//Get all bmp file in Current Directory
	char filename[MAX_FILE][MAX_FILE_NAME];
	TCHAR NPath[MAX_PATH];
	//char* locjpg = "C:\\Users\\MIAT\\Desktop\\MIAT\\1051master\\Embedded_vision\\HW1\\DevC++\\*.jpg";
	//char* locbmp = "C:\\Users\\MIAT\\Desktop\\MIAT\\1051master\\Embedded_vision\\HW1\\DevC++\\*.bmp"; 
	char locbmp[256];	
	char type[20] = {"\\*.bmp"};
	int fcnt;
    clock_t start;
    double duration;
	
	GetCurrentDirectory(MAX_PATH, NPath);
	cout << "GetCurrentDirectory: " << NPath << endl;
	strcpy(locbmp, (char*)NPath);
	strcat(locbmp, type);
 	fcnt = FindFile(locbmp, filename);
 	
 	for(int index = 0;index < fcnt;index++){
		//ignore "n" filename
		bool condition = strstr(filename[index], "processed") > 0 || strstr(filename[index], "-HE") > 0 
						 || strstr(filename[index], "-SHE") > 0;
		if(condition){
			cout << "ignore filename: " << filename[index] << endl;
			continue;
		} else	cout << "filename: " << filename[index] << endl;
		
		isvalid = Read_BMP(filename[index], ima, nr, nc);
		if (!isvalid) continue;
		bima=UC2D(nr,nc);
		Write_BMP_8bits("ima.bmp", ima, nr, nc);
		
		sprintf(ofilename, "%s", filename[index]);
		strcpy(ofilename + strlen(ofilename) - 4, "-HE.bmp");
		
    	start = std::clock();
		HE(ima,bima,nr,nc);
   		duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
    	std::cout<<"HE: "<< duration <<'\n';
    	
		Write_BMP_8bits(ofilename, bima, nr, nc);
		//AHE(ima,bima,nr,nc);
		//Write_BMP_8bits("AHE.bmp", bima, nr, nc);
		//DSHE(ima,bima,nr,nc, 1.0f);
		//Write_BMP_8bits("DSHE.bmp", bima, nr, nc);
		sprintf(ofilename, "%s", filename[index]);
		strcpy(ofilename + strlen(ofilename) - 4, "-SHE.bmp");
		
    	start = std::clock();
		SHE(ima,bima,nr,nc);
   		duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
    	std::cout << "SHE: " << duration << '\n';
    	
		Write_BMP_8bits(ofilename, bima, nr, nc);
		//DPHE(ima,bima,nr,nc);
		//Write_BMP_8bits(ofilename, bima, nr, nc);
		sprintf(ofilename, "%s", filename[index]);
		strcpy(ofilename + strlen(ofilename) - 4, "-processed.bmp");
		
    	start = std::clock();
		ADPHE_SHE(ima,bima,nr,nc);
   		duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
    	std::cout<< "ADPHE_SHE: " << duration << '\n';
    	
		Write_BMP_8bits(ofilename, bima, nr, nc);
	}
	cout << "\nProgram done.\n";
	system("PAUSE");
	return 1;
}

void HE(unsigned char **ima, unsigned char **bima, int nr,int nc){
	int bin[GRAY_LEVEL] = {0};
	const int N = nr * nc; //total number of image pixel
	
	//accumulative pixels
	for(int i = 0;i < nr;i++)
		for(int j = 0;j < nc;j++)
			bin[ima[i][j]]++;
			
	//CDF(Cumulative Distribution Function)
	for(int i = 1;i < GRAY_LEVEL;i++)
		bin[i] += bin[i - 1], bin[i - 1] = (GRAY_LEVEL - 1) * bin[i - 1] / N;
		
	bin[GRAY_LEVEL - 1] = (GRAY_LEVEL - 1) * bin[GRAY_LEVEL - 1] / N;
	
	//redistribute image pixel
	for(int i = 0;i < nr;i++)
		for(int j = 0;j < nc;j++)
			bima[i][j] = bin[ima[i][j]];
	
}

//http://fanli7.net/a/JAVAbiancheng/ANT/20130408/336280.html
int CDF(int *bin, const int left, const int right){
	
	//CDF(Cumulative Distribution Function)
	for(int i = left + 1;i <= right;i++)
		bin[i] += bin[i - 1];
		
	return bin[right];
}

void AHE(unsigned char **ima, unsigned char **bima, int nr,int nc){
	const int BLOCK = 80;
	const int BLOCK_2 = BLOCK >> 1;
	int bin[GRAY_LEVEL] = {0};
	const int N = BLOCK * BLOCK; //total number of image pixel
	
	for(int i = BLOCK_2;i < nr - BLOCK_2;i++)
		for(int j = BLOCK_2;j < nc - BLOCK_2;j++){
			//reset bin count
			memset(bin, 0, sizeof(bin));
			
			//accumulative pixels
			for(int k = -BLOCK_2;k < BLOCK_2;k++)
				for(int l = -BLOCK_2;l < BLOCK_2;l++)
					bin[ima[i + k][j + l]]++;
					
			//CDF(Cumulative Distribution Function)
			int count = CDF(bin, 0, ima[i][j]);
			
			//redistribute image pixel
			bima[i][j] = (GRAY_LEVEL - 1) * count / N;
		}
}

void CLAHE(unsigned char **ima, unsigned char **bima, int nr,int nc, int ratio){
	
}

struct BIN{
	int count;
	int label;
};

struct GROUP{
	int left;
	int right;
};

void init(BIN* bin){
	for(int i = 0;i < GRAY_LEVEL;i++)
		bin[i].count = 0, bin[i].label = i;
}

void accumulating_histogram(BIN *bin, unsigned char **ima, int nr, int nc){
	//accumulative pixels
	for(int i = 0;i < nr;i++)
		for(int j = 0;j < nc;j++)
			bin[ima[i][j]].count++;
}

void density(BIN *bin, int *d){
	const int WIN_SIZE = 6;
	const int WIN_SIZE_2 = (WIN_SIZE >> 1) - 1;
	int buf[WIN_SIZE] = {0};
	int head = 0;
	int count = 0;
	
	for(int index = 0;index < GRAY_LEVEL + WIN_SIZE_2;index++){
		count -= buf[head];
		if(index < GRAY_LEVEL){
			count += bin[index].count;
			buf[head] = bin[index].count;
		}
		if(index >= WIN_SIZE_2) d[index - WIN_SIZE_2] = count;
		
		head = (head + 1) % WIN_SIZE;
	}
}

void quicksort(BIN *arr, int left, int right){
	if(left >= right) return;
	
	int pivot = arr[left].count;
	int swap_index = right;
	int ptr = right;
	int tmp;
	while(left < ptr){
		if(pivot > arr[ptr].count){
			tmp = arr[swap_index].count;
			arr[swap_index].count = arr[ptr].count;
			arr[ptr].count = tmp;
			
			tmp = arr[swap_index].label;
			arr[swap_index].label = arr[ptr].label;
			arr[ptr].label = tmp;
			swap_index--;
		}
		ptr--;
	}
	
	tmp = arr[swap_index].count;
	arr[swap_index].count = arr[left].count;
	arr[left].count = tmp;
	
	tmp = arr[swap_index].label;
	arr[swap_index].label = arr[left].label;
	arr[left].label = tmp;
	
	quicksort(arr, left, swap_index - 1);
	quicksort(arr, swap_index + 1, right);
}

void labeling(BIN *bin, int *d){
	BIN *bin_copy;
	
	bin_copy = (BIN*)malloc(sizeof(BIN) * GRAY_LEVEL);
	memcpy(bin_copy, bin, sizeof(BIN) * GRAY_LEVEL);
	
	for(int index = 0;index < GRAY_LEVEL;index++){
		bin_copy[index].count = d[index];
	}
	
	quicksort(bin_copy, 0, GRAY_LEVEL - 1);
	
	for(int index = 0;index < GRAY_LEVEL;index++){
		bin[bin_copy[index].label].label = index;
	}
	
	free(bin_copy);
}

int group(int *d, GROUP *group_pair, const float C){
	const int WIN_SIZE = 6;
	int AD = 0;
	int D;
	int ws = 1;
	int group_cnt = 0;
		
	for(int i = 0;i < GRAY_LEVEL;i++){
		int momentary;
		if(i == 0 || i == GRAY_LEVEL) momentary = d[i] + d[i + 1];
		else momentary = d[i - 1] + d[i + 1];
		
		AD += momentary;
		D = (int)(C * (float)AD) / (WIN_SIZE + (ws << 1));
		
		if(momentary > D) ws++;	
		else group_pair[group_cnt].left = i - ws, group_pair[group_cnt].right = i - 1, ws = 1, group_cnt++, AD = 0;
	}
	group_pair[0].left = 0;
	group_pair[group_cnt].left = GRAY_LEVEL - ws, group_pair[group_cnt].right = GRAY_LEVEL - 1, group_cnt++;
	
	return group_cnt;
}

void HE(unsigned char **ima, unsigned char **bima, BIN *bin, GROUP *group_pair, int group_cnt, int nr, int nc){
	
	FILE *in, *out;
	in = fopen("in_histogram.csv", "w");
	out = fopen("out_histogram.csv", "w");
	int arr[GRAY_LEVEL] = {0};
	
	if(!in || !out) printf("fail to open file\n");
	
	//print input image's histogram
	
	for(int index = 0;index < GRAY_LEVEL;index++)
		fprintf(in, "%d, %d\n", index, bin[index].count);
	
	//region individual histogram equalization to individual region
	int MAX_GRAY_LEVEL;
	int partial_sum, prev_partial_sum = 0;
	for(int group = 0;group < group_cnt;group++){
		partial_sum = bin[group_pair[group].left].count;
		
		for(int index = group_pair[group].left + 1;index <= group_pair[group].right;index++)
			partial_sum += bin[index].count, bin[index].count += bin[index - 1].count;
		
		MAX_GRAY_LEVEL = group_pair[group].right - group_pair[group].left;
		//partial_sum -= prev_partial_sum * (group_pair[group].right - group_pair[group].left + 1);
		for(int index = group_pair[group].left;index <= group_pair[group].right;index++)
			bin[index].count = MAX_GRAY_LEVEL * bin[index].count / partial_sum + group_pair[group].left;
		
		//prev_partial_sum += partial_sum;
	}
	
	/*
	for(int index = 0;index < GRAY_LEVEL;index++){
		printf("[%d] = %d\n", index, bin[index].count);
	}*/
	
	for(int i = 0;i < nr;i++)
		for(int j = 0;j < nc;j++)
			bima[i][j] = (unsigned char)bin[ima[i][j]].count, arr[bima[i][j]]++;
			
	for(int index = 0;index < GRAY_LEVEL;index++)
		fprintf(out, "%d, %d\n", index, arr[index]);
			
	fclose(in);
	fclose(out);
}

//refer to Image Contrast Enhancement based Sub-histogram Equalization Technique without Over-equalization Noise 
void DSHE(unsigned char **ima, unsigned char **bima, int nr, int nc, const float C){
	const int N = nr * nc; //total number of image pixel
	BIN bin[GRAY_LEVEL] = {0};
	int d[GRAY_LEVEL] = {0};
	GROUP group_pair[GRAY_LEVEL] = {0};
	int group_cnt;
	
	//initial BIN
	init(bin);
	
	//generate histogram of input image
	accumulating_histogram(bin, ima, nr, nc);
	
	//desity of histogram
	density(bin, d);
	
	//descending label density
	labeling(bin, d);
	
	//range of sub-histogram
	group_cnt = group(d, group_pair, C);
	/*
	group_cnt = 3;
	group_pair[0].left = 0; group_pair[0].right = 46;
	group_pair[1].left = 47; group_pair[1].right = 195;
	group_pair[2].left = 196; group_pair[2].right = 255;
	printf("group_cnt = %d\n", group_cnt);
	*/
	
	for(int index = 0;index < group_cnt;index++){
		printf("group%d is %d to %d\n", index, group_pair[index].left, group_pair[index].right);
	}
	
	//remove unnecessary group
	
	
	//sub-histogram equalization
	HE(ima, bima, bin, group_pair, group_cnt, nr, nc);
}

void SHE(unsigned char **ima, unsigned char **bima, int nr, int nc){
	int bin[GRAY_LEVEL] = {0};
	int last_second = 0;
	int last_second_index = 0;
	int index = 0;
	int N = nr * nc;
	
	//accumulating_histogram
	for(int i = 0;i < nr;i++)
		for(int j = 0;j < nc;j++) 
			bin[ima[i][j]]++;
	
	//delete last second two element
	for(index = GRAY_LEVEL - 1;index >= 0;index--)
		if(bin[index]){
			if(last_second == 1){
				N = N - (bin[last_second_index] - bin[index]);
				bin[last_second_index] = bin[index];
			}else if(last_second == 0){
				last_second_index = index;	
			}
			last_second++;	
		}
	
	//replace the first appear pixel count to zero
	for(index = 0;index < GRAY_LEVEL;index++)
		if(bin[index]){
			N = N - bin[index];
			bin[index] = 0;
			break;
		}
	
	for(index = 1;index < GRAY_LEVEL;index++)
		bin[index] += bin[index - 1], bin[index - 1] = (GRAY_LEVEL - 1) * bin[index - 1] / N;
	
	bin[GRAY_LEVEL - 1] = (GRAY_LEVEL - 1) * bin[GRAY_LEVEL - 1] / N;
	
	/*for(index = 0;index < GRAY_LEVEL;index++){
		printf("[%d] = %d\n", index, bin[index]);
	}*/
	
	//redistribute image pixel
	for(int i = 0;i < nr;i++)
		for(int j = 0;j < nc;j++)
			bima[i][j] = bin[ima[i][j]];
	
}

int local_maximum(int *bin){
	const int WIN_SIZE = 9;
	const int WIN_SIZE_2 = (WIN_SIZE >> 1) - 1;
	int buf[WIN_SIZE] = {0};
	int cnt = 0;
	int head = 0;
	int max;
	int max_sum = 0;
	int max_avg;
	
	for(int index = 0;index < GRAY_LEVEL;index++){
		buf[head] = bin[index];
		
		if(index >= WIN_SIZE - 1){
			max = buf[0];
			for(int i = 1;i < WIN_SIZE;i++){
				if(max < buf[i]){
					max = buf[i];
				}
			}
			
			if(buf[WIN_SIZE_2] >= max){
				max_sum += max;
				cnt++;
			}
		}
		
		head = (head + 1) % WIN_SIZE;
	}
	
	max_avg = max_sum / cnt;
	
	return max_avg;
}

int min(int a, int b){
	return (a >= b) ? b : a;
}

void DPHE(unsigned char **ima, unsigned char **bima, int nr, int nc){
	int bin[GRAY_LEVEL] = {0};
	int index;
	int N = nr * nc;
	int L; //non-zero element of histogram
	//int Tup = (int)((float)N * 0.20f), Tdown = (int)((float)N * 0.005f);
	int Tup, Tdown;
	
	//accumulating_histogram
	for(int i = 0;i < nr;i++)
		for(int j = 0;j < nc;j++)
			bin[ima[i][j]]++;
	
	L = 0;
	for(index = 0;index < GRAY_LEVEL;index++){
		if(bin[index]) L++;	
	}
	
	//update Tup and Tdown
	Tup = local_maximum(bin);
	Tdown = min(nr * nc, Tup * L) / GRAY_LEVEL;
	
	N = 0;
	for(index = 0;index < GRAY_LEVEL;index++){
		if(bin[index] > 0){
			if(bin[index] >= Tup){			//avoid over-equalization
				bin[index] = Tup;			
			}else if(bin[index] <= Tdown){ 	//preserve the brightness value
				bin[index] = Tdown;
			}
			N += bin[index];
		}
	}
	
	printf("total = %d, Tup = %d, Tdown = %d\n", N, Tup, Tdown);
	//printf("total = %d, Tup = %d\n", N, Tup);
	for(index = 0;index < GRAY_LEVEL;index++){
		printf("histogram [%d] = %d\n", index, bin[index]);
	}
	
	for(index = 1;index < GRAY_LEVEL;index++)
		bin[index] += bin[index - 1], bin[index - 1] = (GRAY_LEVEL - 1) * bin[index - 1] / N;
	
	bin[GRAY_LEVEL - 1] = (GRAY_LEVEL - 1) * bin[GRAY_LEVEL - 1] / N;
	
	for(index = 0;index < GRAY_LEVEL;index++){
		printf("LUT [%d] = %d\n", index, bin[index]);
	}
	
	//redistribute image pixel
	for(int i = 0;i < nr;i++)
		for(int j = 0;j < nc;j++)
			bima[i][j] = bin[ima[i][j]];
	
}

void accumulating_histogram(unsigned char **ima, int *bin, int nr, int nc){
	//accumulating_histogram
	for(int i = 0;i < nr;i++)
		for(int j = 0;j < nc;j++)
			bin[ima[i][j]]++;
}

void SHE(int *bin){
	int index;
	int last_second = 0;
	int last_second_index = 0;
	//delete last second two element
	for(index = GRAY_LEVEL - 1;index >= 0;index--)
		if(bin[index]){
			if(last_second == 1){
				//N = N - (bin[last_second_index] - bin[index]);
				bin[last_second_index] = bin[index];
			}else if(last_second == 0){
				last_second_index = index;	
			}
			last_second++;	
		}
	
	//replace the first appear pixel count to zero
	for(index = 0;index < GRAY_LEVEL;index++)
		if(bin[index]){
			//N = N - bin[index];
			bin[index] = 0;
			break;
		}
}

int non_zero_cnt(int *bin){
	int index, L;
	L = 0;
	for(index = 0;index < GRAY_LEVEL;index++){
		if(bin[index]) L++;	
	}
	return L;
}

int threshold_histogram(int *bin, int Tup, int Tdown){
	int N, index;
	N = 0;
	for(index = 0;index < GRAY_LEVEL;index++){
		if(bin[index] > 0){
			if(bin[index] >= Tup){			//avoid over-equalization
				bin[index] = Tup;			
			}
			else if(bin[index] <= Tdown){ 	//preserve the brightness value
				bin[index] = Tdown;
			}
			N += bin[index];
		}
	}
	return N;
}

void accumulating(int *bin, int N){
	int index;
	for(index = 1;index < GRAY_LEVEL;index++)
		bin[index] += bin[index - 1], bin[index - 1] = (GRAY_LEVEL - 1) * bin[index - 1] / N;
	
	bin[GRAY_LEVEL - 1] = (GRAY_LEVEL - 1) * bin[GRAY_LEVEL - 1] / N;
}

void LUT(unsigned char **ima, unsigned char **bima, int *bin, int nr, int nc){
	//redistribute image pixel
	for(int i = 0;i < nr;i++)
		for(int j = 0;j < nc;j++)
			bima[i][j] = bin[ima[i][j]];
}

void ADPHE_SHE(unsigned char **ima, unsigned char **bima, int nr, int nc){
	int bin[GRAY_LEVEL] = {0};
	int index;
	int N = nr * nc;
	int L; //non-zero element of histogram
	//int Tup = (int)((float)N * 0.20f), Tdown = (int)((float)N * 0.005f);
	int Tup, Tdown;
	
	//get input Histogram
	accumulating_histogram(ima, bin, nr, nc);
	
	//SHE
	SHE(bin);
	
	//DPHE
	L = non_zero_cnt(bin);
	Tup = local_maximum(bin);
	
	Tdown = min(nr * nc, Tup * L) / (GRAY_LEVEL << 1);
	
	N = threshold_histogram(bin, Tup, Tdown);
	
	accumulating(bin, N);
	
	LUT(ima, bima, bin, nr, nc);
}
