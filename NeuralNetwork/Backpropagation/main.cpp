#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <time.h>
#include <windows.h>

//user define
#include "array_1d.h"
#include "array_2d.h"
#include "array2d.h"
#include "array.h"
#include "debug_msg.h"

#define NUM_OF_INPUT 4
#define NUM_OF_HIDDEN_LAYER 3
#define NUM_OF_HIDDEN_NEURON 6
#define NUM_OF_OUTPUT 3

#define IW_SIZE NUM_OF_INPUT * NUM_OF_HIDDEN_NEURON
#define HW_SIZE (NUM_OF_HIDDEN_LAYER - 1) * NUM_OF_HIDDEN_NEURON * NUM_OF_HIDDEN_NEURON
#define OW_SIZE NUM_OF_HIDDEN_NEURON * NUM_OF_OUTPUT

#define LEARN_RATE 0.2f
#define NUM_OF_ITERATION 1000
#define FITNESS_THRESHOLD 0.0001f

typedef struct{
	float IW[IW_SIZE];
	float HW[HW_SIZE];
	float OW[OW_SIZE];
}NN;

float sigmoid(float x){
	return 1.0f / (1.0f + exp(-x));
}

void bp(NN *w, float *x, float *y, float *d, float *HO){
	int i, j, k;
	float sum;
	float delta[NUM_OF_HIDDEN_LAYER * NUM_OF_HIDDEN_NEURON + NUM_OF_OUTPUT];
	
	//[0 ~ NUM_OF_HIDDEN_LAYER * NUM_OF_HIDDEN_NEURON] is hidden layer delta
	//[NUM_OF_HIDDEN_LAYER * NUM_OF_HIDDEN_NEURON ~ NUM_OF_HIDDEN_LAYER * NUM_OF_HIDDEN_NEURON + OW_SIZE] is output layer delta
	
//	printf("%d\n", NUM_OF_HIDDEN_LAYER * NUM_OF_HIDDEN_NEURON + NUM_OF_OUTPUT);
	for(i = 0;i < NUM_OF_OUTPUT;i++){
		delta[i + NUM_OF_HIDDEN_LAYER * NUM_OF_HIDDEN_NEURON] = (d[i] - y[i]) * (1 - y[i]) * y[i];
	}
	//2: 1 0
	for(i = NUM_OF_HIDDEN_LAYER - 1;i >= 0;i--){
		for(j = 0;j < NUM_OF_HIDDEN_NEURON;j++){
			sum = 0.0f;
			if(i == NUM_OF_HIDDEN_LAYER - 1)
				for(k = 0;k < NUM_OF_OUTPUT;k++)
					sum += delta[k + NUM_OF_HIDDEN_LAYER * NUM_OF_HIDDEN_NEURON] * w->OW[k * NUM_OF_HIDDEN_NEURON + j];
			else
				for(k = 0;k < NUM_OF_HIDDEN_NEURON;k++)
					sum += delta[k + (i + 1) * NUM_OF_HIDDEN_NEURON] * 
							w->HW[i * NUM_OF_HIDDEN_NEURON * NUM_OF_HIDDEN_NEURON + k * NUM_OF_HIDDEN_NEURON + j];
			
			delta[i * NUM_OF_HIDDEN_NEURON + j] = sum * HO[i * NUM_OF_HIDDEN_NEURON + j] * (1.0f - HO[i * NUM_OF_HIDDEN_NEURON + j]);
		}
	}
	
	//update weight
	for(i = 0;i < NUM_OF_OUTPUT;i++){
		for(j = 0;j < NUM_OF_HIDDEN_NEURON;j++){
			w->OW[i * NUM_OF_HIDDEN_NEURON + j] +=  
				LEARN_RATE * delta[i + NUM_OF_HIDDEN_LAYER * NUM_OF_HIDDEN_NEURON] * HO[j + (NUM_OF_HIDDEN_LAYER - 1) * NUM_OF_HIDDEN_NEURON];
		}
	}
	
	for(i = 1;i <= (NUM_OF_HIDDEN_LAYER - 1);i++){
		for(j = 0;j < NUM_OF_HIDDEN_NEURON;j++){
			for(k = 0;k < NUM_OF_HIDDEN_NEURON;k++){
				w->HW[(i - 1) * NUM_OF_HIDDEN_NEURON * NUM_OF_HIDDEN_NEURON + j * NUM_OF_HIDDEN_NEURON + k]
					+= LEARN_RATE * delta[j + i * NUM_OF_HIDDEN_NEURON] * HO[(i - 1) * NUM_OF_HIDDEN_NEURON + k];
			}
		}
	}
	
	for(i = 0;i < NUM_OF_HIDDEN_NEURON;i++){
		for(j = 0;j < NUM_OF_INPUT;j++){
			w->IW[i * NUM_OF_INPUT + j] = w->IW[i * NUM_OF_INPUT + j] + LEARN_RATE * x[j] * delta[i];
		}
	}
	
}

void forward(NN *w, float *x, float *y, float *d, float * HO){
	int i, j, k;
	float v, sum;
	float *wptr;
	
	//HO is 
	
	wptr = w->IW;
	for(i = 0;i < NUM_OF_HIDDEN_NEURON;i++){
		sum = 0.0f;
		for(j = 0;j < NUM_OF_INPUT;j++){
			sum += w->IW[i * NUM_OF_INPUT + j] * x[j];
		}
		HO[i] = sigmoid(sum);
		DEBUG("v = %f\n", sum);
		DEBUG("HO[%d] = %f\n", i, HO[i]);
	}
	
	for(i = 0;i < (NUM_OF_HIDDEN_LAYER - 1);i++){
		for(j = 0;j < NUM_OF_HIDDEN_NEURON;j++){
			sum = 0.0f;
			for(k = 0;k < NUM_OF_HIDDEN_NEURON;k++){
				sum += w->HW[i * NUM_OF_HIDDEN_NEURON * NUM_OF_HIDDEN_NEURON + j * NUM_OF_HIDDEN_NEURON + k]
						 * HO[i * NUM_OF_HIDDEN_NEURON + k];
			}
			HO[(i + 1) * NUM_OF_HIDDEN_NEURON + j] = sigmoid(sum);
			DEBUG("v = %f\n", sum);
			DEBUG("HO[%d] = %f\n", (i + 1) * NUM_OF_HIDDEN_NEURON + j, HO[(i + 1) * NUM_OF_HIDDEN_NEURON + j]);
		}
	}
	
	v = 0.0f;
	for(i = 0;i < NUM_OF_OUTPUT;i++){
		sum = 0.0f;
		for(j = 0;j < NUM_OF_HIDDEN_NEURON;j++){
			sum += w->OW[i * NUM_OF_HIDDEN_NEURON + j] * 
					HO[(NUM_OF_HIDDEN_LAYER - 1) * NUM_OF_HIDDEN_NEURON + j];
		}
		y[i] = sigmoid(sum);
		DEBUG("v = %f\n", sum);
		DEBUG("y[%d] = %f\n", i, y[i]);
	}
	
}

void read_sample(char *filename, float ***in, float ***out, int *sample_size){
	int i, j;
	int isize, osize;
	FILE *fptr;
	
	fptr = fopen(filename, "r");
	
	fscanf(fptr, "%d %d %d", sample_size, &isize, &osize);
	
	*in = f2D_Initialize(*sample_size, isize);
	*out = f2D_Initialize(*sample_size, osize);
	
	for(i = 0;i < *sample_size;i++){
		printf("sample = %d\n", i);
		for(j = 0;j < isize;j++){
			fscanf(fptr, "%f", &(*in)[i][j]);
			DEBUG("%f\n", (*in)[i][j]);
		}
		
		for(j = 0;j < osize;j++){
			fscanf(fptr, "%f", &(*out)[i][j]);
			DEBUG("%f\n", (*out)[i][j]);
		}
	}
	
}

float rand_float(){
	return ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
}

void random_arr(NN *w){
	int i;
	for(i = 0;i < IW_SIZE;i++){
		w->IW[i] = rand_float();
	}
	for(i = 0;i < HW_SIZE;i++){
		w->HW[i] = rand_float();
	}
	for(i = 0;i < OW_SIZE;i++){
		w->OW[i] = rand_float();
	}
}

void print_weight(NN *w){
	int i;
	printf("------------------\n");
	for(i = 0;i < IW_SIZE;i++){
		printf("IW[%d] = %f\n", i, w->IW[i]);
	}
	for(i = 0;i < HW_SIZE;i++){
		printf("HW[%d] = %f\n", i, w->HW[i]);
	}
	for(i = 0;i < OW_SIZE;i++){
		printf("OW[%d] = %f\n", i, w->OW[i]);
	}
	printf("------------------\n");
}

float fitness_func(float *y, float *d, int size){
	int i;
	float fitness;
	fitness = 0.0f;
	
	for(i = 0;i < size;i++){
		float tmp;
		tmp = (y[i] - d[i]);
		fitness += tmp * tmp;
	}
	fitness *= 0.5f;
	
	return fitness;
}

void reorder(int *order, int sample_size){
	int i, swap, index;
	
	//re-order
	for(i = 0;i < sample_size;i++){
		index = rand() % sample_size;
		swap = order[index];
		order[index] = order[i];
		order[i] = swap;
	}
}

void train_bp(char *filename, NN *w){
	int i, j;
	int sample_size;
	int *order;
	float fitness_min, fitness;
	float **in, **out;
	float o[NUM_OF_OUTPUT];
	float HO[NUM_OF_HIDDEN_LAYER * NUM_OF_HIDDEN_NEURON]; //hidden layer output
	
	random_arr(w);
	print_weight(w);
	read_sample(filename, &in, &out, &sample_size);
	
	order = (int*)i1D_Initialize(sample_size);
	for(i = 0;i < sample_size;i++)
		order[i] = i;
	
	//train
	fitness_min = FLT_MAX;
	for(i = 0;i < NUM_OF_ITERATION && fitness_min > FITNESS_THRESHOLD;i++){
		
		// randomize sample
		reorder(order, sample_size);
		
		// backpropagation
		fitness = 0.0f;
		for(j = 0;j < sample_size;j++){
			forward(w, in[order[j]], o, out[order[j]], HO);
			bp(w, in[order[j]], o, out[order[j]], HO);
			
			// find minimum fitness function
			fitness += fitness_func(o, out[order[j]], NUM_OF_OUTPUT);
		}
		
		if(fitness_min >= fitness)
			fitness_min = fitness;
		printf("[%d] = %f\n", i, fitness_min / sample_size);
		Sleep(50);
	}
	
	// test
	for(i = 0;i < sample_size;i++){
		forward(w, in[i], o, out[i], HO);
		printf("input = ");
		for(j = 0;j < NUM_OF_INPUT;j++){
			printf("%.3f, ", in[i][j]);
		}
		printf("output ");
		for(j = 0;j < NUM_OF_OUTPUT;j++){
			printf("%.3f, ", o[j]);
		}
		printf("expect ");
		for(j = 0;j < NUM_OF_OUTPUT;j++){
			printf("%.3f, ", out[i][j]);
		}
		printf("\n");
	}
	
	print_weight(w);
	
}

int main(){
	
	int i, j;
	NN w;
	
	srand(time(NULL));
	
	train_bp("iris.txt", &w);
	
	return 0;
}
