#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_OF_SAMPLE 3
#define NUM_OF_INPUT 25
#define LEARN_RATE 0.8f
#define ITERATION 100
#define NUM_OF_TEST_SAMPLE 3

//to be continued...
//改成自動讀取輸入和期望輸出 
//三個數字，用三個感知機分三類 

typedef struct{
	float weight[NUM_OF_INPUT];
} Perceptron; 

float in[NUM_OF_SAMPLE][NUM_OF_INPUT] = {{0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0}, 
										 {0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0}, 
										 {0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0}};
					 
float ans1[NUM_OF_SAMPLE] = {1, -1, -1};	 
float ans2[NUM_OF_SAMPLE] = {-1, 1, -1};	 
float ans3[NUM_OF_SAMPLE] = {-1, -1, 1};

float test_in[NUM_OF_SAMPLE][NUM_OF_INPUT] = {{0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0}, 
										 	  {0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0}, 
										 	  {0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0}};
		
float test_ans[NUM_OF_TEST_SAMPLE][NUM_OF_SAMPLE] = {{1, -1, -1},
													  {-1, 1, -1},
													  {-1, -1, 1}};

Perceptron p1, p2, p3;

float sgn(float x){
	return x >= 0.0f ? 1.0f : -1.0f;
}

float forward(float *x, float *w){
	int i;
	float summation;
	
	summation = 0.0f;
	for(i = 0;i < NUM_OF_INPUT;i++){
		summation += x[i] * w[i];
	}
	
	return sgn(summation);
}

void modify_weight(float *x, float *w, float sign){
	
	int i;
	for(i = 0;i < NUM_OF_INPUT;i++){
		w[i] = w[i] + sign * LEARN_RATE * x[i];
	}
	
}

void modify_weight_LMS(float *x, float *w, float ans, float y){
	
	int i;
	for(i = 0;i < NUM_OF_INPUT;i++){
		w[i] = w[i] - LEARN_RATE * x[i] * (y - ans);
	}
	
}

void random_arr(float *w){
	int i;
	for(i = 0;i < NUM_OF_INPUT;i++){
		w[i] = (rand() / RAND_MAX) * 2.0f - 1.0f;
	}
}

void print_weight(float *w){
	int i;
	printf("------------------\n");
	for(i = 0;i < NUM_OF_INPUT;i++){
		printf("w[%d] = %f\n", i, w[i]);
	}
}

void train(Perceptron *p, float in[NUM_OF_SAMPLE][NUM_OF_INPUT], float *ans){
	
	int i, ptr, iter;
	int modified;
	float sign;
	//srand(time(0));
	random_arr(p->weight);
	
	iter = ptr = modified = 0;
	while(modified++ != NUM_OF_SAMPLE && iter < ITERATION){
		sign = forward(in[ptr], p->weight);
		
		if(sign != ans[ptr]){
			modified = 0;
			//perceptron learning
			//modify_weight(in[ptr], weight, ans[ptr]);
			//printf("modified...\n");
		}
		
		//LMS learning
		modify_weight_LMS(in[ptr], p->weight, ans[ptr], sign);
		printf("modified...\n");
		
		printf("%d, %f, %f", iter, sign, ans[ptr]);
		print_weight(p->weight);
		
		ptr = (ptr + 1) % NUM_OF_SAMPLE;
		iter++;
	}
	
}

void test(Perceptron *p, float in[NUM_OF_TEST_SAMPLE][NUM_OF_INPUT], float *ans){
	int i;
	float sign;
	for(i = 0;i < NUM_OF_TEST_SAMPLE;i++){
		sign = forward(in[i], p->weight);
		printf("%d, %f == %f?\n", i, sign, ans[i]);
	}
}

int main(){
	srand(time(NULL));
	
	printf("training perceptron 1...\n");
	train(&p1, in, ans1);
	printf("training perceptron 2...\n");
	train(&p2, in, ans2);
	printf("training perceptron 3...\n");
	train(&p3, in, ans3);
	
	//test
	test(&p1, test_in, test_ans[0]);
	test(&p2, test_in, test_ans[1]);
	test(&p3, test_in, test_ans[2]);
	
	system("PAUSE");
	return 0;
}
