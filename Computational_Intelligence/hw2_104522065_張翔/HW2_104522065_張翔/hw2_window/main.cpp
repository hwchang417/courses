#include <windows.h>	 //windows interface
#include <stdio.h>
#include <stdlib.h>		
#include <string.h>		
#include <tchar.h>		
#include <unistd.h>		
#include <pthread.h>	//multi-thread program
#include <conio.h>		
#include <dos.h>		
#include <math.h> 
#include <Commdlg.h> 	//open file dialog
#include "CCanvas.h" 	//drawing

#include <time.h>

static int ITERATION_COUNT = 1000; 				//iteration count
#define NEURAL_COUNT 20							//J: fi and w //多顆會更難調整, 少顆無法逼近 
static int NUM_OF_GENE = 500;					//group size
#define NUM_OF_INPUT_DIMENSION 3 				//p, forward, right, left
static float mutation_prob = 0.1f; 				//Pm, 突變機率 
static float crossover_prob = 0.6f; 			//Pc, 交配機率 
#define theta_value 1.0f 						//theta of F function
#define random_noise_size_normalized 0.1f 		//
#define random_noise_size_non_normalized 0.2f	//非正規化的隨機雜訊振福 
#define finish_threshold 50.0f 				//結束閥值 
#define random_gene_generation NUM_OF_GENE/20.0f //根據迭代次數加入隨機NUM_OF_GENE / 2 to 0個gene
#define m_range 30.0f							//0~30.0f
#define sigma_range 10.0f						//0~10.0f
#define w_range 80.0f							//-40~40.0f

//display
#define WINDOWS_WIDTH 640	//視窗寬度 
#define WINDOWS_HEIGHT 480	//視窗高度 
#define OFFSET_X 200		//x相對偏移量 
#define OFFSET_Y 400		//y相對偏移量 
#define CAR_R 60			//車子相對半徑 3.0 * 20 
#define BLACK 0x000000		//顏色常數 
#define WHITE 0xffffff

#define CAR_RADIUS 3.0f
#define max(a, b) ((a > b) ? a : b)
#define min(a, b) ((a <= b) ? a : b)
#define car_length (CAR_RADIUS * 2.0f)
#define PI 3.14159265359f
 
 //座標點結構 
struct Point{
	float x, y;
	float distance;
};

//基因結構 
struct GENE{
	float F;
	float fi; 		//zero fi
	float sigma[NEURAL_COUNT]; 	
	float w[NEURAL_COUNT]; 		
	float m[NEURAL_COUNT][NUM_OF_INPUT_DIMENSION]; 		
};

//thread running state 
#define BUFFER_LEN 256
enum THREAD_STATE{
	THREAD_RESET, THREAD_START, THREAD_STOP, THREAD_FINISH
};

float x, y;
float DF, DR, DL, degree;
float theta;
int flag = 0;

char textstring[50];
char training_file[80];
char training_result[80];
char textX[] = {"x"};
char textY[] = {"y"};
char textDeg[] = {"deg"};
char textIter[] = {"iter"};
char textGENE[] = {"GeNum"};
char textPc[] = {"Pc"};
char textPm[] = {"Pm"};
char Arial[] = {"Arial"};	// display font
char szFile[MAX_PATH] = ""; // buffer for file name 

//locus
int locus_cnt;
struct Point locus[200];

/*  global NN parameter  */
struct GENE best_gene;

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
char fuClassName[] = "GA";
char fuWindowName[] = "GA simulation";
char textSave[20] = {""};
HWND btn_reset, btn_open, btn_training, btn_start, btn_stop, textField_x, textField_y, textField_deg, textField_info, textField_filename;
HWND textBox_x, textBox_y, textBox_deg, textBox_iter, textBox_gene, textBox_pc, textBox_pm, textField_result;
THREAD_STATE thread_start_flag;

/*  thread for gene calculation  */
pthread_t thread_a;
pthread_mutex_t mutex;

float abs(float num);
int training_NN();
int getFileDirectory(char* szFile);
void random_gene_init(struct GENE* gene);
void calculate_output(struct GENE& gene, float* x);
void reproduct_roulette_wheel_selection(float* fitness, int* crossover_pool, int start_pos, int end_pos);
void reproduct_contention_selection(float* fitness, int* crossover_pool, int start_pos, int end_pos);
void reproduct_top_N(float* fitness, int* crossover_pool, int start_pos, int end_pos);
void reproduct(float* fitness, int* crossover_pool);
void crossover_operation(struct GENE& new_gene1, struct GENE& new_gene2, struct GENE& gene1, struct GENE& gene2, const float one_minus_degree);
void crossover(struct GENE* gene, int* crossover_pool, struct GENE* dst, int times_of_iter);
void mutation(struct GENE* gene, int times_of_iter);
void freeMemory();
float point2point(float x1, float y1, float x2, float y2);
struct Point getDF();
struct Point getDR();
struct Point getDL();
void init(float tmpx, float tmpy, float tmp_deg);
void plot(HWND &hwnd, float tmp_x, float tmp_y, float deg, Point forward, Point right, Point left);
void main_thread(HWND &hwnd);
void* test_func(void* arg);

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nFunsterStil)

{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */
    HANDLE hin;
    DWORD WriteWord;
	int res;
    
    const char buffer[BUFFER_LEN] = "Hello, World!!";
    hin = GetStdHandle(STD_OUTPUT_HANDLE);

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = fuClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default color as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           fuClassName,         /* Classname */
           fuWindowName,       /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           WINDOWS_WIDTH,       /* The programs width */
           WINDOWS_HEIGHT,      /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );
           
    //MessageBox(NULL, "Window Creation Failed!", "Error!",
    //    MB_ICONEXCLAMATION | MB_OK);
        
    if(hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    /* Make the window visible on the screen */
    ShowWindow (hwnd, nFunsterStil);
    
    /* user code */
	thread_start_flag = THREAD_RESET;
	res = pthread_create(&thread_a, NULL, test_func, NULL);
	if(res != 0){
		perror("Thread creation failed.");
		exit(EXIT_FAILURE);
	}
	//res = pthread_join(thread_a, NULL); //wait for thread termination
	//printf("Thread return value: %s\n", (char*)thread_result);
    
    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    TCHAR greeting[] = _T("Hello, World!");
    CCanvas canvas;
    int res, training_ok;
	clock_t begin, end;
	double time_spent;
					
    
    switch (message){                  /* handle the messages */
        case WM_CREATE:
        	//WS_CHILD: whether it is inside parent window
            textField_info = CreateWindow("STATIC",
                                     "",
                                     WS_VISIBLE | WS_CHILD,
                                     0, 90, 325, 20,
                                     hwnd, NULL, NULL, NULL);
                                     
            textField_filename = CreateWindow("STATIC",
                                     "",
                                     WS_VISIBLE | WS_CHILD,
                                     0, 110, 325, 20,
                                     hwnd, NULL, NULL, NULL);
            textField_result = CreateWindow("STATIC",
                                     "",
                                     WS_VISIBLE | WS_CHILD,
                                     0, 130, 325, 20,
                                     hwnd, NULL, NULL, NULL);
            /*textField_x = CreateWindow("STATIC",
                                     "x",
                                     WS_VISIBLE | WS_CHILD,
                                     5, 5, 30, 20,
                                     hwnd, NULL, NULL, NULL);
            textField_y = CreateWindow("STATIC",
                                     "y",
                                     WS_VISIBLE | WS_CHILD,
                                     55, 5, 30, 20,
                                     hwnd, NULL, NULL, NULL);
            textField_deg = CreateWindow("STATIC",
                                     "deg",
                                     WS_VISIBLE | WS_CHILD,
                                     100, 5, 30, 20,
                                     hwnd, NULL, NULL, NULL);*/ 
            textBox_x = CreateWindow( "EDIT",
									"0", //display text
									WS_VISIBLE | WS_BORDER | WS_CHILD | ES_NUMBER,
									5, 30, 30, 20,
									hwnd, NULL, NULL, NULL);
            textBox_y = CreateWindow( "EDIT",
									"0", //display text
									WS_VISIBLE | WS_BORDER | WS_CHILD | ES_NUMBER,
									55, 30, 30, 20,
									hwnd, NULL, NULL, NULL);
            textBox_deg = CreateWindow( "EDIT",
									"90", //display text
									WS_VISIBLE | WS_BORDER | WS_CHILD | ES_NUMBER,
									100, 30, 30, 20,
									hwnd, NULL, NULL, NULL);
									
            textBox_iter = CreateWindow( "EDIT",
									"1000", //display text
									WS_VISIBLE | WS_BORDER | WS_CHILD | ES_NUMBER,
									145, 30, 45, 20,
									hwnd, NULL, NULL, NULL);
            textBox_gene = CreateWindow( "EDIT",
									"500", //display text
									WS_VISIBLE | WS_BORDER | WS_CHILD | ES_NUMBER,
									195, 30, 45, 20,
									hwnd, NULL, NULL, NULL);
            textBox_pc = CreateWindow( "EDIT",
									"0.6", //display text
									WS_VISIBLE | WS_BORDER | WS_CHILD | ES_NUMBER,
									250, 30, 30, 20,
									hwnd, NULL, NULL, NULL);
            textBox_pm = CreateWindow( "EDIT",
									"0.1", //display text
									WS_VISIBLE | WS_BORDER | WS_CHILD | ES_NUMBER,
									295, 30, 30, 20,
									hwnd, NULL, NULL, NULL);
									
			btn_reset = CreateWindow("BUTTON", //name
                                  "reset",//text
                                  WS_VISIBLE | WS_CHILD | WS_BORDER | BS_PUSHBUTTON | WS_TABSTOP,
                                  5, 60, 40, 20, //x, y, w, h
                                  hwnd, (HMENU) 1, NULL, NULL); //signal number
			btn_open = CreateWindow("BUTTON", //name
                                  "openFile",//text
                                  WS_VISIBLE | WS_CHILD | WS_BORDER | BS_PUSHBUTTON | WS_TABSTOP,
                                  50, 60, 60, 20, //x, y, w, h
                                  hwnd, (HMENU) 2, NULL, NULL); //signal number*/
			btn_training = CreateWindow("BUTTON", //name
                                  "training",//text
                                  WS_VISIBLE | WS_CHILD | WS_BORDER | BS_PUSHBUTTON | WS_TABSTOP,
                                  120, 60, 60, 20, //x, y, w, h
                                  hwnd, (HMENU) 3, NULL, NULL); //signal number*/
			btn_start = CreateWindow("BUTTON", //name
                                  "start",//text
                                  WS_VISIBLE | WS_CHILD | WS_BORDER | BS_PUSHBUTTON | WS_TABSTOP ,
                                  190, 60, 40, 20, //x, y, w, h
                                  hwnd, (HMENU) 4, NULL, NULL); //signal number
			btn_stop = CreateWindow("BUTTON", //name
                                  "stop",//text
                                  WS_VISIBLE | WS_CHILD | WS_BORDER | BS_PUSHBUTTON | WS_TABSTOP,
                                  240, 60, 40, 20, //x, y, w, h
                                  hwnd, (HMENU) 5, NULL, NULL); //signal number*/
			
			
			pthread_mutex_lock(&mutex); 
			SetFocus(textBox_x); //A handle to the window that will receive the keyboard input. If this parameter is NULL, keystrokes are ignored.
			EnableWindow(btn_reset, FALSE);
			EnableWindow(btn_open, TRUE);
			EnableWindow(btn_training, FALSE);
			EnableWindow(btn_start, FALSE);
			EnableWindow(btn_stop, FALSE);
			thread_start_flag = THREAD_RESET;
			pthread_mutex_unlock(&mutex);
			//plot
			
		break;
		//when event was happend, it get this messages WM_COMMAND.
		case WM_COMMAND:
			switch(LOWORD(wParam)){ //LOWORD(): get low 16 bits of 32 bits
				case 1://reset
					pthread_mutex_lock(&mutex);
					thread_start_flag = THREAD_RESET;
					SetWindowText(textBox_x, "0");
					SetWindowText(textBox_y, "0");
					SetWindowText(textBox_deg, "90");
					EnableWindow(btn_start, TRUE);
					EnableWindow(btn_stop, FALSE);
					pthread_mutex_unlock(&mutex);
					
					//MessageBox(handle control, content, windows name, button type);
					//MessageBox(hwnd, textSave, "Button", MB_OK);
					
				break;
				case 2://open file
				//open file dialog => enable training button
	   			    char* result;
					pthread_mutex_lock(&mutex); 
					if(getFileDirectory(szFile)){
					    result = strrchr(szFile, '\\');
						if(result == NULL)
							SetWindowText(textField_filename, szFile);
						else{
							result++;
							sprintf(training_file, "測試資料: %s", result);
							SetWindowText(textField_filename, training_file);
						}
					}
					EnableWindow(btn_training, TRUE);
					pthread_mutex_unlock(&mutex);
					
				break;
				case 3://training
				//train the gene
				//training button => enable running button
				
					pthread_mutex_lock(&mutex); 
					sprintf(textstring, "訓練中請稍候, 若程式無法回應為正常狀況");
					SetWindowText(textField_info, textstring);
					pthread_mutex_unlock(&mutex);
					
					begin = clock();
					/* here, do your time-consuming job */
	   			    training_ok = training_NN();
					end = clock();
					time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
					
	   			    if(training_ok){
						pthread_mutex_lock(&mutex); 
						sprintf(textstring, "訓練中完成, 花費%.4f s", time_spent);
						SetWindowText(textField_info, textstring);
						pthread_mutex_unlock(&mutex);
						pthread_mutex_lock(&mutex); 
						EnableWindow(btn_start, TRUE);
						pthread_mutex_unlock(&mutex);
					}else{
						pthread_mutex_lock(&mutex); 
						sprintf(textstring, "訓練中失敗, 無法配置記憶體");
						SetWindowText(textField_info, textstring);
						pthread_mutex_unlock(&mutex);
					}
				
				break;
				case 4://start
				//running button => start thread
				//error beep sound
					MessageBeep(MB_ICONERROR);
					
					pthread_mutex_lock(&mutex); 
					EnableWindow(btn_start, FALSE);
					EnableWindow(btn_reset, TRUE);
					EnableWindow(btn_stop, TRUE);
					thread_start_flag = THREAD_START;
					pthread_mutex_unlock(&mutex);
					
					
					
					//do stuff here
					//create a thread to update screen, and go to next position
					//when stop button was pressed, stop thread
					
				break;
				case 5://stop
					pthread_mutex_lock(&mutex); 
					EnableWindow(btn_start, TRUE);
					EnableWindow(btn_stop, FALSE);
					thread_start_flag = THREAD_STOP;
					pthread_mutex_unlock(&mutex);
				break;
				default:
				break;
			}
		break;
        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);
			//ClearScreen();
            //canvas.rectangle(0, 0, 640, 480, 0x000000, 0x000000, 0);
            canvas.init(hdc); //set current hdc to canvas class
            
		    canvas.text(5, 5, &textX[0], &Arial[0], 0, BLACK);
		    canvas.text(55, 5, &textY[0], &Arial[0], 0, BLACK);
		    canvas.text(100, 5, &textDeg[0], &Arial[0], 0, BLACK);
		    canvas.text(145, 5, &textIter[0], &Arial[0], 0, BLACK);
		    canvas.text(190, 5, &textGENE[0], &Arial[0], 0, BLACK);
		    canvas.text(250, 5, &textPc[0], &Arial[0], 0, BLACK);
		    canvas.text(295, 5, &textPm[0], &Arial[0], 0, BLACK);
            /*canvas.line(x++, y++, 100, 100, 0x000000, 4);
            canvas.ellipse(100, 100, 50, 50, 0x000000, 0x0000FF, 4);
			
			*/
			// Here your application is laid out.
            // For this introduction, we just print out "Hello, World!"
            // in the top left corner.
            //TextOut(handle control, x, y, textString, len)
            //TextOut(hdc, 5, 5, greeting, _tcslen(greeting));
            
            // End application-specific layout section.
            EndPaint(hwnd, &ps);
        break;
        //case WM_LBUTTONDOWN:
        //    MessageBox(hwnd, "hello again", "box title", MB_OK);
        //    break;  
		case WM_KEYDOWN:
			int virtual_code;
			int key_bits;
			virtual_code = (int) wParam;
			key_bits = (int) lParam;
			
			switch (virtual_code){
				case VK_RIGHT: { } break;
				case VK_LEFT: { } break;
				case VK_TAB: { } break;
				default: break;
				return (0);
			}
		break;
        case WM_DESTROY:
			 thread_start_flag = THREAD_FINISH;
             PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
        break;
        default:                      /* for messages that we don't deal with */
        return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

float abs(float num){
	return num >= 0.0f ? num : -num;
}

int getFileDirectory(char* szFile){
	
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = "All Files (*.*)\0*.*\0Text Files (*.txt)\0*.txt\0";
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	//ofn.lpstrDefExt = "txt";
	if(GetOpenFileName(&ofn))
	{
	   // Do something usefull with the filename stored in szFileName 
	   //printf("%s", szFile);
	   return 1;
	}
	
   MessageBox(NULL, szFile, "Error!",
        MB_ICONEXCLAMATION | MB_OK);
   return 0;
}

int training_NN(){
	//get input from window
	char buf[20];
	GetWindowText(textBox_iter, buf, 20);
	ITERATION_COUNT = (int)atoi(buf);
	GetWindowText(textBox_gene, buf, 20);
	NUM_OF_GENE = (float)atoi(buf);
	GetWindowText(textBox_pc, buf, 20);
	crossover_prob = (float)atof(buf);
	GetWindowText(textBox_pm, buf, 20);
	mutation_prob = (float)atof(buf);
	
	//NN
	float x[NUM_OF_INPUT_DIMENSION];
	struct GENE* gene = (struct GENE*)malloc(sizeof(struct GENE) * NUM_OF_GENE);//[NUM_OF_GENE]
	struct GENE* tmp = (struct GENE*)malloc(sizeof(struct GENE) * NUM_OF_GENE); //[NUM_OF_GENE]
	float* E = (float*)malloc(sizeof(float) * NUM_OF_GENE);						//[NUM_OF_GENE]
	int* crossover_pool = (int*)malloc(sizeof(int) * NUM_OF_GENE);				//[NUM_OF_GENE]
	float min_mse;
	float mean_error;
	//check memory
	if(gene == NULL || tmp == NULL || E == NULL || crossover_pool == NULL){
		printf("Memory allocation error\n");
		return 0;
	}
	
	//read input data from file
	FILE* in, *result;
	result = fopen("result.csv", "w");
	in = fopen(szFile, "r");
	if(in == NULL){
		printf("read file error with directory %s\n", szFile);
	}
	if(result == NULL){
		printf("read file error with directory\n");
	}
	fprintf(result, "ITERATION_COUNT = %d\n", ITERATION_COUNT);
	fprintf(result, "NEURAL_COUNT = %d\n", NEURAL_COUNT);
	fprintf(result, "NUM_OF_GENE = %d\n", NUM_OF_GENE);
	fprintf(result, "NUM_OF_INPUT_DIMENSION = %d\n", NUM_OF_INPUT_DIMENSION);
	fprintf(result, "mutation_prob = %.3f\n", mutation_prob);
	fprintf(result, "crossover_prob = %.3f\n", crossover_prob);
	fprintf(result, "theta_value = %.3f\n", theta_value);
	fprintf(result, "random_noise_size_normalized = %.3f\n", random_noise_size_normalized);
	fprintf(result, "random_noise_size_non_normalized = %.3f\n", random_noise_size_non_normalized);
	fprintf(result, "finish_threshold = %.3f\n", finish_threshold);
	
	//set random seed with present time
	srand(time(0));
	
	//common varialbe
	int i, j, k, n, min_index, finish_flag = 0;
	float f, r, l, theta, Fsum, min;
	random_gene_init(gene);
	for(j = 0;j <= ITERATION_COUNT && !finish_flag;j++){
		for(i = 0;i < NUM_OF_GENE;i++) E[i] = 0.0f;
		mean_error = 0.0f;
		//rewind file pointer to the beginning of file
		rewind(in);
		//training
		while(fscanf(in, "%f %f %f %f\n", &f, &r, &l, &theta) != -1){
			
			//read data, and minus car radius
			x[0] = f - 3.0f;
			x[1] = r - 3.0f;
			x[2] = l - 3.0f;
			theta = - theta;
			//input to NN
			float error_diff;
			for(k = 0;k < NUM_OF_GENE;k++){
				//RBFN
				calculate_output(gene[k], x);
				//fitness function, gene[k].F為第k個基因對輸入資料的適應函數值總和 
				//when value is smaller, crossover prob is higher 
				error_diff = gene[k].F - theta;
				mean_error += abs(error_diff);
				error_diff = error_diff * error_diff;
				E[k] += (error_diff)/2.0f;
			}
		}
		//check whether mean error less than threshold
		if(mean_error <= finish_threshold){
			memcpy(&best_gene, &gene[k], sizeof(struct GENE));
			printf("mean error is less than %.4f\n", mean_error);
			finish_flag = 1;
		}
		rewind(in);
		
		min = E[0];
		min_index = 0;
		for(k = 1;k < NUM_OF_GENE;k++){
			if(E[k] <= min){
				min = E[k];
				min_index = k;
			}
			//printf("E[%d]=%.4f, gene[%d].F = %.4f\n", k, E[k], k, gene[k].F);
		}
		pthread_mutex_lock(&mutex); 
	    sprintf(training_result, "目前最好基因的平方誤差(E)=%.4f", E[min_index]);
		SetWindowText(textField_result, training_result);	
		pthread_mutex_unlock(&mutex);
		
		fprintf(result, "%d,%.4f\n", j, min);
		if(j < ITERATION_COUNT){
			
			if(mean_error <= finish_threshold){
				memcpy(&best_gene, &gene[k], sizeof(struct GENE));
				printf("mean error is less than %.4f\n", mean_error);
				finish_flag = 1;
			}
			
			//fitness function 1/E, summation and normalization		
			//reproduct_roulette_wheel_selection(E, crossover_pool, 0, NUM_OF_GENE);
			//reproduct_contention_selection(E, crossover_pool, 0, NUM_OF_GENE); //挑NUM_OF_GENE個 
			reproduct(E, crossover_pool);
			
			//crossover: real type with w = w + t(d - y); with Pm(everyone only have one mate)
			memcpy(tmp, gene, NUM_OF_GENE * sizeof(struct GENE));
			crossover(tmp, crossover_pool, gene, j); //now_gene, crossover_pool, new_gene, iteration_count
			
			//mutation: select one for mutation with Pc(change the value x = x + s*random_noise, s is the size of noise)
			mutation(gene, j); //now_gene, iteration_count
			
			//_sleep(500);
		}//else{
			
		//}
		//_sleep(500);
	}
	
	if(!finish_flag){
		memcpy(&best_gene, &gene[min_index], sizeof(struct GENE));
	}
	fprintf(result, "mean error is %.4f\n", mean_error / 55.0f);
	fprintf(result, "f,r,l,theta,y\n", f, r, l, theta, best_gene.F);
		
	fprintf(result, "best_gene.E = %.3f\n", E[min_index]);
	fclose(in);
	fclose(result);
	
	free(gene);
	free(tmp); 
	free(E);	
	free(crossover_pool);				
	
	printf("Training complete!\n");
	return 1;
}

void random_gene_init(struct GENE* gene){
	int i, j, k;
	float* lookup;
	const float inc = 1.0f/RAND_MAX;
	//printf("RAND_MAX=%d\n", RAND_MAX);
	lookup = (float*)malloc(sizeof(float) * RAND_MAX);
	lookup[0] = 0.0f;
	for(i = 1;i < RAND_MAX;i++) lookup[i] = lookup[i-1] + inc;//, printf("lookup[%d] = %.6f\n", i, lookup[i]);
	
	for(i = 0;i < NUM_OF_GENE;i++){
		for(j = 0;j < NEURAL_COUNT;j++){
			gene[i].sigma[j] = sigma_range * lookup[rand()]; //0~10
			gene[i].w[j] = w_range * lookup[rand()] - w_range / 2.0f; //-40~40
			for(k = 0;k < NUM_OF_INPUT_DIMENSION;k++) gene[i].m[j][k] = m_range * lookup[rand()]; //0~30
			
			//printf("gene[%d] => sigma[%d]=%.2f, w[%d]=%.2f\n", i, j, gene[i].sigma[j], j, gene[i].w[j]);
		}
		gene[i].F = 0.0f; //0~40 => 0~1
		gene[i].fi = theta_value; //1
		gene[i].w[0] = theta_value; //1
	}
	free(lookup);
}

void calculate_output(struct GENE& gene, float* x){
	int i, j;
	//F = summation(w*fi);
	float convolution_sum = gene.fi * gene.w[0];
	float diff, diff_sum;
	float sigma2;
	
	for(i = 1;i < NEURAL_COUNT;i++){
		diff_sum = 0.0f;
		sigma2 = 2.0f*gene.sigma[i]*gene.sigma[i];
		for(j = 0;j < NUM_OF_INPUT_DIMENSION;j++){
			diff = x[j] + CAR_RADIUS - gene.m[i][j];
			diff = diff * diff;
			diff_sum += diff;
		}
		//diff_sum = sqrt(diff_sum) * sqrt(diff_sum); //don't need sqrt
		convolution_sum += gene.w[i] * exp(-diff_sum/sigma2);
		//printf("diff_sum=%.2f, sigma2=%.2f\n", diff_sum, sigma2);
	}
	gene.F = convolution_sum;
	//printf("convolution_sum=%.2f\n", convolution_sum);
}

void reproduct_roulette_wheel_selection(float* fitness, int* crossover_pool, int start_pos, int end_pos){
	int i, k;
	float rnd, Fsum = 0.0f;
	//normalization
	for(k = 0;k < NUM_OF_GENE;k++){
		//avoid divide zero
		if(fitness[k]>=0.00001f)fitness[k] = 1.0f/fitness[k];	
		else fitness[k] = 100000.0f;
		Fsum += fitness[k];
	}
	fitness[0] = fitness[0] / Fsum;
	for(k = 1;k < NUM_OF_GENE;k++){
		fitness[k] = fitness[k] / Fsum;
		fitness[k] = fitness[k] + fitness[k - 1];
		//printf("fitness[%d] = %f\n", k, fitness[k]);
	}
	//random select 
	for(k = start_pos;k < end_pos;k++){ //選十次 
		rnd = ((float)rand())/RAND_MAX;
		for(i = 0;i < NUM_OF_GENE;i++){
			if(rnd < fitness[i]){
				crossover_pool[k] = i;
				//printf("rnd = %f, crossover num = %d\n", rnd, i);	
				i = NUM_OF_GENE;
			}
		}
	}
	/*for(k = 0;k < NUM_OF_GENE;k++){
		printf("crossover_pool[%d]=%d\n", k, crossover_pool[k]);
	}*/
}

void reproduct_contention_selection(float* fitness, int* crossover_pool, int start_pos, int end_pos){
	int rnd1, k, rnd2;
	//random select two gene, and choose the bigger one.
	for(k = start_pos;k < end_pos;k++){ //選NUM_OF_GENE次 
		rnd1 = rand() % NUM_OF_GENE;
		rnd2 = rand() % NUM_OF_GENE;
		crossover_pool[k] = (fitness[rnd1] <= fitness[rnd2]) ? rnd1 : rnd2;
		//printf("rnd1 = %d, rnd2 = %d, crossover num = %d\n", rnd1, k, crossover_pool[k]);	
	}
}

void reproduct_top_N(float* fitness_in, int* crossover_pool, int start_pos, int end_pos){
	int i, j, k, flag;
	float* fitness = (float*)malloc(sizeof(float) * NUM_OF_GENE);
	int* num = (int*)malloc(sizeof(int) * NUM_OF_GENE);
	
	if(num == NULL || fitness == NULL){
		printf("Memory allocation error!");
		exit(1);
	}
	memcpy(fitness, fitness_in, sizeof(float) * NUM_OF_GENE);
	for(i = 0;i < NUM_OF_GENE;i++) num[i] = i;
		
	//for(i = 0;i < NUM_OF_GENE;i++) printf("%d, %.3f: ", num[i], fitness[i]);
	//printf("\n");
	//bubble sort
	flag = 1;
	for(i = 0;i < NUM_OF_GENE && flag;i++){
		flag = 0;
		for(j = 0, k = NUM_OF_GENE-1-i;j < k;j++){
			if(fitness[j] > fitness[j+1]){
				float tempf = fitness[j];
				fitness[j] = fitness[j+1];
				fitness[j+1] = tempf;
				int tempi = num[j];
				num[j] = num[j+1];
				num[j+1] = tempi;
				flag = 1;
			}
		}
	}
	//for(i = 0;i < NUM_OF_GENE;i++) printf("%d, %.3f: ", num[i], fitness[i]);
	//printf("\n");
	printf("min = %.3f\n", fitness[num[0]]);
	//選最小適應函數值的 end_pos - start_pos個 
	for(k = start_pos;k < end_pos;k++){
		crossover_pool[k] = num[k - start_pos];
	}
	free(num);
	free(fitness);
}

void reproduct(float* fitness, int* crossover_pool){
	int contention_end = NUM_OF_GENE / 2; //50%
	int top_N_end = contention_end + NUM_OF_GENE / 2;//50%
	reproduct_contention_selection(fitness, crossover_pool, 0, contention_end); 
	reproduct_top_N(fitness, crossover_pool, contention_end, top_N_end); 
}

void crossover_operation(struct GENE& new_gene1, struct GENE& new_gene2, struct GENE& gene1, struct GENE& gene2, const float one_minus_degree){
	int j, k;
	float rnd_near_far, rnd_degree;
	rnd_near_far = (float)rand() / RAND_MAX; //if greater than 0.5, go far from each other gene
	rnd_degree = (float)rand() / RAND_MAX * 0.5f;
	rnd_degree = rnd_degree * one_minus_degree;
	//printf("rnd_near_far=%.3f, rnd_degree=%.3f\n", rnd_near_far, rnd_degree);
	if(rnd_near_far >= 0.5){//far
		if(gene1.fi >= gene2.fi){
			new_gene1.fi = gene1.fi + rnd_degree * (gene1.fi - gene2.fi);
			new_gene2.fi = gene2.fi - rnd_degree * (gene1.fi - gene2.fi);
		}else{
			new_gene1.fi = gene1.fi - rnd_degree * (gene2.fi - gene1.fi);
			new_gene2.fi = gene2.fi + rnd_degree * (gene2.fi - gene1.fi);
		}
		
		for(k = 0;k < NEURAL_COUNT;k++){
			rnd_degree = (float)rand() / RAND_MAX * 0.5f;
			rnd_degree = rnd_degree * one_minus_degree;
			if(gene1.sigma[k] >= gene2.sigma[k]){ //第k顆neural 
				new_gene1.sigma[k] = gene1.sigma[k] + rnd_degree * (gene1.sigma[k] - gene2.sigma[k]);
				new_gene2.sigma[k] = gene2.sigma[k] - rnd_degree * (gene1.sigma[k] - gene2.sigma[k]);
			}else{
				new_gene1.sigma[k] = gene1.sigma[k] - rnd_degree * (gene2.sigma[k] - gene1.sigma[k]);
				new_gene2.sigma[k] = gene2.sigma[k] + rnd_degree * (gene2.sigma[k] - gene1.sigma[k]);
			}
			
			rnd_degree = (float)rand() / RAND_MAX * 0.5f;
			rnd_degree = rnd_degree * one_minus_degree;
			if(gene1.w[k] >= gene2.w[k]){ //第k顆neural 的weight 
				new_gene1.w[k] = gene1.w[k] + rnd_degree * (gene1.w[k] - gene2.w[k]);
				new_gene2.w[k] = gene2.w[k] - rnd_degree * (gene1.w[k] - gene2.w[k]);
			}else{
				new_gene1.w[k] = gene1.w[k] - rnd_degree * (gene2.w[k] - gene1.w[k]);
				new_gene2.w[k] = gene2.w[k] + rnd_degree * (gene2.w[k] - gene1.w[k]);
			}
			
			for(j = 0;j < NUM_OF_INPUT_DIMENSION;j++){
				rnd_degree = (float)rand() / RAND_MAX * 0.5f;
				rnd_degree = rnd_degree * one_minus_degree;
				if(gene1.m[k][j] >= gene2.m[k][j]){ //第k顆neural 的mk, ex: m1, m2, m3
					new_gene1.m[k][j] = gene1.m[k][j] + rnd_degree * (gene1.m[k][j] - gene2.m[k][j]);
					new_gene2.m[k][j] = gene2.m[k][j] - rnd_degree * (gene1.m[k][j] - gene2.m[k][j]);
				}else{
					new_gene1.m[k][j] = gene1.m[k][j] - rnd_degree * (gene2.m[k][j] - gene1.m[k][j]);
					new_gene2.m[k][j] = gene2.m[k][j] + rnd_degree * (gene2.m[k][j] - gene1.m[k][j]);
				}
			}
			//printf("gene[%d] => sigma[%d]=%.2f, w[%d]=%.2f\n", i, j, gene[i].sigma[j], j, gene[i].w[j]);
		}
		//printf("far new_gene1.fi=%.4f, new_gene2.fi=%.4f, gene1.fi=%.4f, gene2.fi=%.4f, degree=%.4f\n", new_gene1.fi, new_gene2.fi, gene1.fi, gene2.fi, rnd_degree);
	
	}else{//near
		if(gene1.fi >= gene2.fi){
			new_gene1.fi = gene1.fi - rnd_degree * (gene1.fi - gene2.fi);
			new_gene2.fi = gene2.fi + rnd_degree * (gene1.fi - gene2.fi);
		}else{
			new_gene1.fi = gene1.fi + rnd_degree * (gene2.fi - gene1.fi);
			new_gene2.fi = gene2.fi - rnd_degree * (gene2.fi - gene1.fi);
		}
		
		for(k = 0;k < NEURAL_COUNT;k++){
			rnd_degree = (float)rand() / RAND_MAX * 0.5f;
			rnd_degree = rnd_degree * one_minus_degree;
			if(gene1.sigma[k] >= gene2.sigma[k]){ //第k顆neural 
				new_gene1.sigma[k] = gene1.sigma[k] - rnd_degree * (gene1.sigma[k] - gene2.sigma[k]);
				new_gene2.sigma[k] = gene2.sigma[k] + rnd_degree * (gene1.sigma[k] - gene2.sigma[k]);
			}else{
				new_gene1.sigma[k] = gene1.sigma[k] + rnd_degree * (gene2.sigma[k] - gene1.sigma[k]);
				new_gene2.sigma[k] = gene2.sigma[k] - rnd_degree * (gene2.sigma[k] - gene1.sigma[k]);
			}
			rnd_degree = (float)rand() / RAND_MAX * 0.5f;
			rnd_degree = rnd_degree * one_minus_degree;
			if(gene1.w[k] >= gene2.w[k]){ //第k顆neural 的weight 
				new_gene1.w[k] = gene1.w[k] - rnd_degree * (gene1.w[k] - gene2.w[k]);
				new_gene2.w[k] = gene2.w[k] + rnd_degree * (gene1.w[k] - gene2.w[k]);
			}else{
				new_gene1.w[k] = gene1.w[k] + rnd_degree * (gene2.w[k] - gene1.w[k]);
				new_gene2.w[k] = gene2.w[k] - rnd_degree * (gene2.w[k] - gene1.w[k]);
			}
			
			for(j = 0;j < NUM_OF_INPUT_DIMENSION;j++){
				rnd_degree = (float)rand() / RAND_MAX * 0.5f;
				rnd_degree = rnd_degree * one_minus_degree;
				if(gene1.m[k][j] >= gene2.m[k][j]){ //第k顆neural 的mk, ex: m1, m2, m3
					new_gene1.m[k][j] = gene1.m[k][j] - rnd_degree * (gene1.m[k][j] - gene2.m[k][j]);
					new_gene2.m[k][j] = gene2.m[k][j] + rnd_degree * (gene1.m[k][j] - gene2.m[k][j]);
				}else{
					new_gene1.m[k][j] = gene1.m[k][j] + rnd_degree * (gene2.m[k][j] - gene1.m[k][j]);
					new_gene2.m[k][j] = gene2.m[k][j] - rnd_degree * (gene2.m[k][j] - gene1.m[k][j]);
				}
			}
			
			//printf("gene[%d] => sigma[%d]=%.2f, w[%d]=%.2f\n", i, j, gene[i].sigma[j], j, gene[i].w[j]);
		}
		//printf("near new_gene1.w[1]=%.4f, new_gene2.w[1]=%.4f, gene1.w[1]=%.4f, gene2.w[1]=%.4f, degree=%.4f\n", new_gene1.w[1], new_gene2.w[1], gene1.w[1], gene2.w[1], rnd_degree);
	}	
	
	//boundary check
	/*new_gene1.fi = (new_gene1.fi >= 1.0f) ? 1.0f : (new_gene1.fi <= 0.0f ? 0.0f : new_gene1.fi);
	new_gene2.fi = (new_gene2.fi >= 1.0f) ? 1.0f : (new_gene2.fi <= 0.0f ? 0.0f : new_gene2.fi);
	
	for(k = 0;k < NEURAL_COUNT;k++){
		new_gene1.sigma[k] = (new_gene1.sigma[k] >= 10.0f) ? 10.0f : (new_gene1.sigma[k] <= 0.0f ? 0.0f : new_gene1.sigma[k]);
		new_gene2.sigma[k] = (new_gene2.sigma[k] >= 10.0f) ? 10.0f : (new_gene2.sigma[k] <= 0.0f ? 0.0f : new_gene2.sigma[k]);
		
		new_gene1.w[k] = (new_gene1.w[k] >= 1.0f) ? 1.0f : (new_gene1.w[k] <= 0.0f ? 0.0f : new_gene1.w[k]);
		new_gene2.w[k] = (new_gene2.w[k] >= 1.0f) ? 1.0f : (new_gene2.w[k] <= 0.0f ? 0.0f : new_gene2.w[k]);
		
		for(j = 0;j < NUM_OF_INPUT_DIMENSION;j++){
			
			new_gene1.m[k][j] = (new_gene1.m[k][j] >= 30.0f) ? 30.0f : (new_gene1.m[k][j] <= 0.0f ? 0.0f : new_gene1.m[k][j]);
			new_gene2.m[k][j] = (new_gene2.m[k][j] >= 30.0f) ? 30.0f : (new_gene2.m[k][j] <= 0.0f ? 0.0f : new_gene2.m[k][j]);
		}
		
		//printf("gene[%d] => sigma[%d]=%.2f, w[%d]=%.2f\n", i, j, gene[i].sigma[j], j, gene[i].w[j]);
	}*/ 
}

void crossover(struct GENE* gene, int* crossover_pool, struct GENE* dst, int now_iteration_count){
	const int times = NUM_OF_GENE / 2;//20 => 10組 
	const float degree = (float)now_iteration_count / ITERATION_COUNT;
	const float one_minus_degree = 1.0f - degree;
	const int random_gene = (int)((float)random_gene_generation * (one_minus_degree)); //根據迭代次數加入NUM_OF_GENE / 2 to 0個gene 
	const int select_gene_number = NUM_OF_GENE - random_gene;
	int i, k, j; 
	int total_gene;
	int* gene_selected_arr;
	float rnd, random_noise;
	struct GENE* new_gene = (struct GENE*)malloc(sizeof(struct GENE) * NUM_OF_GENE);
	
	printf("%d %d\n", random_gene, select_gene_number);
	//put noise into crossover_pool
	
	for(i = 0;i < select_gene_number;i++){
		random_noise = (float)rand() / RAND_MAX * 2.0f - 1.0f;
		//random_noise = (float)rand() / RAND_MAX - 0.5f;
		gene[i].fi = gene[i].fi + random_noise;
		for(k = 0;k < NEURAL_COUNT;k++){
			//第k顆neural 
			random_noise = (float)rand() / RAND_MAX * 2.0f - 1.0f;
			gene[i].sigma[k] = gene[i].sigma[k] + random_noise * one_minus_degree;
			//第k顆neural 的weight 
			random_noise = (float)rand() / RAND_MAX * 2.0f - 1.0f;
			gene[i].w[k] = gene[i].w[k] + random_noise * one_minus_degree;
			for(j = 0;j < NUM_OF_INPUT_DIMENSION;j++){
				//第k顆neural 的mk, ex: m1, m2, m3
				random_noise = (float)rand() / RAND_MAX * 2.0f - 1.0f;
				gene[i].m[k][j] = gene[i].m[k][j] + random_noise * one_minus_degree;
			}
		}
	}
	
	//根據crossover_pool內的配對數字兩兩進行交配 
	for(i = 0, k = 0, j = 0;i < times;i++){ //抽i次 
		rnd = (float)rand() / RAND_MAX;
		if(rnd <= crossover_prob){//do crossover
			//printf("k=%d k+1=%d\n", k, k+1);
			crossover_operation(new_gene[j], new_gene[j+1], gene[crossover_pool[k]], gene[crossover_pool[k+1]], one_minus_degree);
			j+=2;
		}
		k += 2;
		//printf("rnd=%.3f\n", rnd);
	}
	
	//從NUM_OF_GENE + NUM_OF_GENE / 2選擇NUM_OF_GENE為新的一代 
	total_gene = j + NUM_OF_GENE;
	gene_selected_arr = (int*)malloc(sizeof(int) * total_gene);
	for(i = 0;i < select_gene_number;i++) gene_selected_arr[i] = 1;
	for(;i < total_gene;i++) gene_selected_arr[i] = 0;
	//printf("j is %d, total_gene is %d\n", j, total_gene);
	
	/*for(i = 0;i < total_gene;i++){
		printf("gene_selected_arr[%d] = %d\n", i, gene_selected_arr[i]);
	}*/
	
	for(i = -total_gene;i < total_gene;i++){
		//exchange
		int random_num1 = rand() % total_gene;
		int random_num2 = rand() % total_gene;
		int temp = gene_selected_arr[random_num1];
		gene_selected_arr[random_num1] = gene_selected_arr[random_num2];
		gene_selected_arr[random_num2] = temp;
	}
	
	/*for(i = 0;i < total_gene;i++){
		printf("gene_selected_arr[%d] = %d\n", i, gene_selected_arr[i]);
	}*/
	
	for(i = 0, k = 0;i < total_gene;i++){
		if(gene_selected_arr[i] == 1){
			if(i >= NUM_OF_GENE) memcpy(&dst[k], &new_gene[i - NUM_OF_GENE], sizeof(struct GENE));
			else memcpy(&dst[k], &gene[i], sizeof(struct GENE));
			k++;
		}
	}
	
	free(gene_selected_arr);
	free(new_gene);
}

void mutation(struct GENE* gene, int times_of_iter){
	const float degree = (float)times_of_iter / ITERATION_COUNT;
	const int random_gene = (int)((float)random_gene_generation * (1.0f - degree)); //根據迭代次數加入NUM_OF_GENE / 2 to 0個gene 
	const int mutation_gene_number = NUM_OF_GENE - random_gene;
	int i, k, j;
	float rnd;
	float random_noise;
	
	for(i = 0;i < mutation_gene_number;i++){
		rnd = (float)rand() / RAND_MAX;
		if(rnd <= mutation_prob){
			random_noise = (float)rand() / RAND_MAX * 2.0f - 1.0f;
			gene[i].fi = gene[i].fi + random_noise * random_noise_size_non_normalized;
			
			for(k = 0;k < NEURAL_COUNT;k++){
				//第k顆neural 
				random_noise = (float)rand() / RAND_MAX * 2.0f - 1.0f;
				gene[i].sigma[k] = gene[i].sigma[k] + random_noise * random_noise_size_non_normalized;
				
				//第k顆neural 的weight 
				random_noise = (float)rand() / RAND_MAX * 2.0f - 1.0f;
				gene[i].w[k] = gene[i].w[k] + random_noise * random_noise_size_non_normalized;
				
				for(j = 0;j < NUM_OF_INPUT_DIMENSION;j++){
					//第k顆neural 的mk, ex: m1, m2, m3
					random_noise = (float)rand() / RAND_MAX * 2.0f - 1.0f;
					gene[i].m[k][j] = gene[i].m[k][j] + random_noise * random_noise_size_non_normalized;
				}
				//printf("gene[%d] => sigma[%d]=%.2f, w[%d]=%.2f\n", i, j, gene[i].sigma[j], j, gene[i].w[j]);
			}
			
			
			//boundary check
			/*gene[i].fi = (gene[i].fi >= 1.0f) ? 1.0f : (gene[i].fi <= 0.0f ? 0.0f : gene[i].fi);
			
			for(k = 0;k < NEURAL_COUNT;k++){
				gene[i].sigma[k] = (gene[i].sigma[k] >= 10.0f) ? 10.0f : (gene[i].sigma[k] <= 0.0f ? 0.0f : gene[i].sigma[k]);
				
				gene[i].w[k] = (gene[i].w[k] >= 1.0f) ? 1.0f : (gene[i].w[k] <= 0.0f ? 0.0f : gene[i].w[k]);
				
				for(j = 0;j < NUM_OF_INPUT_DIMENSION;j++){
					gene[i].m[k][j] = (gene[i].m[k][j] >= 30.0f) ? 30.0f : (gene[i].m[k][j] <= 0.0f ? 0.0f : gene[i].m[k][j]);
				}
				
				//printf("gene[%d] => sigma[%d]=%.2f, w[%d]=%.2f\n", i, j, gene[i].sigma[j], j, gene[i].w[j]);
			}*/ 
			
		}
	}
	
	//random new_gene to now_gene
	for(;i < NUM_OF_GENE;i++){
		for(j = 0;j < NEURAL_COUNT;j++){
			gene[i].sigma[j] = sigma_range * (float)rand() / RAND_MAX; //0~10
			gene[i].w[j] = w_range * (float)rand() / RAND_MAX - w_range / 2.0f; //-40~40
			for(k = 0;k < NUM_OF_INPUT_DIMENSION;k++) gene[i].m[j][k] = m_range * (float)rand() / RAND_MAX; //0~30
			
			//printf("gene[%d] => sigma[%d]=%.2f, w[%d]=%.2f\n", i, j, gene[i].sigma[j], j, gene[i].w[j]);
		}
		gene[i].F = 0.0f; //0~40 => 0~1
		gene[i].fi = theta_value; //1
		gene[i].w[0] = theta_value; //1
	}
}

float point2point(float x1, float y1, float x2, float y2){
	return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

struct Point getDF(){
	//const float m = tan((degree - 45.0f)/180.0f*PI);
    //const float b = y - m * x;
    const float r = 0.05;
    const float dx = r * cos((degree)/180.0f*PI);
    const float dy = r * sin((degree)/180.0f*PI);
    int out_of_boundary = 0;
	float dis = 0, tempx = x, tempy = y;
    struct Point boundary_point;
    //先測試單一程式, 判斷有無過邊界 => 代入變零
 
    while(!out_of_boundary){
  		tempx += dx;
  		tempy += dy;
  		
  		if((tempx >= 6.0f && tempy <= 10.0f) || tempx >= 30.0f)
  			out_of_boundary = 1;
		else if(tempy >= 22.0f && tempx <= 18.0f || tempx <= -6.0f)
  			out_of_boundary = 1;
  		
	    //printf("x=%.3f, y=%.3f\n", tempx, tempy);
	    //_sleep(50);
	}
	boundary_point.x = tempx;
	boundary_point.y = tempy;
	
	tempx = tempx - x;
	tempy = tempy - y;
	tempx = tempx * tempx;
	tempy = tempy * tempy;
    boundary_point.distance = sqrt(tempx + tempy);
    //temp = min(min(d1, d3), min(d4, d5));
    
	return boundary_point;
}
struct Point getDR(){
	//const float m = tan((degree - 45.0f)/180.0f*PI);
    //const float b = y - m * x;
    const float r = 0.05;
    const float dx = r * cos((degree - 45.0f)/180.0f*PI);
    const float dy = r * sin((degree - 45.0f)/180.0f*PI);
    int out_of_boundary = 0;
	float dis = 0, tempx = x, tempy = y;
    struct Point boundary_point;
    //先測試單一程式, 判斷有無過邊界 => 代入變零
 
    while(!out_of_boundary){
  		tempx += dx;
  		tempy += dy;
  		
  		if((tempx >= 6.0f && tempy <= 10.0f) || tempx >= 30.0f)
  			out_of_boundary = 1;
		else if(tempy >= 22.0f && tempx <= 18.0f || tempx <= -6.0f)
  			out_of_boundary = 1;
  		
	    //printf("x=%.3f, y=%.3f\n", tempx, tempy);
	    //_sleep(50);
	}
	boundary_point.x = tempx;
	boundary_point.y = tempy;
	
	tempx = tempx - x;
	tempy = tempy - y;
	tempx = tempx * tempx;
	tempy = tempy * tempy;
    boundary_point.distance = sqrt(tempx + tempy);
    //temp = min(min(d1, d3), min(d4, d5));
    
	return boundary_point;
}
struct Point getDL(){
	//const float m = tan((degree - 45.0f)/180.0f*PI);
    //const float b = y - m * x;
    const float r = 0.05;
    const float dx = r * cos((degree + 45.0f)/180.0f*PI);
    const float dy = r * sin((degree + 45.0f)/180.0f*PI);
    int out_of_boundary = 0;
	float dis = 0, tempx = x, tempy = y;
    struct Point boundary_point;
    
    //先測試單一程式, 判斷有無過邊界 => 代入變零
 
    while(!out_of_boundary){
  		tempx += dx;
  		tempy += dy;
  		
  		if((tempx >= 6.0f && tempy <= 10.0f) || tempx >= 30.0f)
  			out_of_boundary = 1;
		else if(tempy >= 22.0f && tempx <= 18.0f || tempx <= -6.0f)
  			out_of_boundary = 1;
  		
	    //printf("x=%.3f, y=%.3f\n", tempx, tempy);
	    //_sleep(50);
	}
	boundary_point.x = tempx;
	boundary_point.y = tempy;
	
	tempx = tempx - x;
	tempy = tempy - y;
	tempx = tempx * tempx;
	tempy = tempy * tempy;
    boundary_point.distance = sqrt(tempx + tempy);
    //temp = min(min(d1, d3), min(d4, d5));
	return boundary_point;
}

void init(float tmpx, float tmpy, float tmp_deg){
    x = tmpx;
	y = tmpy;
    DF = DR = DL = 0.0f;
    degree = tmp_deg;
	theta = 0.0f;    
}

void plot(HWND &hwnd, float tmp_x, float tmp_y, float deg, Point forward, Point right, Point left){
    HDC hdc;
    PAINTSTRUCT ps;
    CCanvas canvas;
    int i;
	const int car_radius = CAR_R / 4;
    //initialization
	//hdc = BeginPaint(hwnd, &ps);
	hdc = GetDC(hwnd);
    canvas.init(hdc); //set current hdc to canvas class
    
	//ClearScreen();
	//display user control
    canvas.rectangle_reverse(0, 0, 325, 150, WINDOWS_WIDTH, WINDOWS_HEIGHT, 0xffffff, 0);
    //canvas.rectangle(0, 0, 639, 479, 0xffffff, 0xffffff, 0);
	
    //draw road: start line
    canvas.line(OFFSET_X - 12 * 10, OFFSET_Y, OFFSET_X + 12 * 10, OFFSET_Y, 0xffffff, 3);
    
    //draw road
    canvas.line(OFFSET_X + 6 * 10, OFFSET_Y, OFFSET_X + 6 * 10, OFFSET_Y - 10 * 10, 0x0000ff, 2);//1
    canvas.line(OFFSET_X - 6 * 10, OFFSET_Y, OFFSET_X - 6 * 10, OFFSET_Y - 22 * 10, 0x0000ff, 2);//2
    canvas.line(OFFSET_X + 6 * 10, OFFSET_Y - 10 * 10, OFFSET_X + 30 * 10, OFFSET_Y - 10 * 10, 0x0000ff, 2);//3
    canvas.line(OFFSET_X - 6 * 10, OFFSET_Y - 22 * 10, OFFSET_X + 18 * 10, OFFSET_Y - 22 * 10, 0x0000ff, 2);//4
    canvas.line(OFFSET_X + 30 * 10, OFFSET_Y - 10 * 10, OFFSET_X + 30 * 10, OFFSET_Y - 45 * 10, 0x0000ff, 2);//5
    canvas.line(OFFSET_X + 18 * 10, OFFSET_Y - 22 * 10, OFFSET_X + 18 * 10, OFFSET_Y - 45 * 10, 0x0000ff, 2);//6
    
    //end point
    canvas.ellipse(OFFSET_X + 24 * 10, OFFSET_Y - 37 * 10, car_radius, car_radius, 0x000000, 0x0000FF, 3);
    
    //locus
    locus[locus_cnt].x = x;
    locus[locus_cnt].y = y;
    locus_cnt++;
    for(i = 0;i < locus_cnt;i++)
		  canvas.ellipse(OFFSET_X + (int)(locus[i].x * 10.0f), OFFSET_Y - (int)(locus[i].y * 10.0f), car_radius, car_radius, 0xFFFFFF, 0x00FF00, 2);
    
    //draw car
	canvas.ellipse(OFFSET_X + (int)(tmp_x * 10.0f), OFFSET_Y - (int)(tmp_y * 10.0f), CAR_R, CAR_R, 0x000000, 0x0000FF, 2);
    canvas.line(OFFSET_X + (int)(tmp_x * 10.0f), OFFSET_Y - (int)(tmp_y * 10.0f), OFFSET_X + (int)(tmp_x * 10.0f) + (int)((float)CAR_R * cos(deg/180.0f*PI)), OFFSET_Y - (int)(tmp_y * 10.0f) - (int)((float)CAR_R * 1.25 * sin(deg/180.0f*PI)), 0xffffff, 2);//6
	
	//line to right boundary
	canvas.line(OFFSET_X + x * 10, OFFSET_Y - y * 10, OFFSET_X + right.x * 10, OFFSET_Y - right.y * 10, 0x00ffff, 3);//6
    
	//line to left boundary
	canvas.line(OFFSET_X + x * 10, OFFSET_Y - y * 10, OFFSET_X + left.x * 10, OFFSET_Y - left.y * 10, 0x00ffff, 3);//6
    
	//line to forward boundary
	canvas.line(OFFSET_X + x * 10, OFFSET_Y - y * 10, OFFSET_X + forward.x * 10, OFFSET_Y - forward.y * 10, 0x00ffff, 3);//6
    
    // End application-specific layout section.
    //EndPaint(hwnd, &ps);
    ReleaseDC(hwnd, hdc);
}

void main_thread(HWND &hwnd){
	FILE* out = NULL;
	//write result to excel file
	char textstring[50];
	float theta_rad, degree_rad;
	Point forward, right, left;
	//get input from textbox
	float tmp_x, tmp_y, tmp_deg;
	char buf[20];
	
	pthread_mutex_lock(&mutex); 
	//get input
	GetWindowText(textBox_x, buf, 20);
	tmp_x = (float)atoi(buf);
	GetWindowText(textBox_y, buf, 20);
	tmp_y = (float)atoi(buf);
	GetWindowText(textBox_deg, buf, 20);
	tmp_deg = (float)atoi(buf);
	init(tmp_x, tmp_y, tmp_deg);
	pthread_mutex_unlock(&mutex);
	locus_cnt = 0;
	
	while(1){
    	if(thread_start_flag == THREAD_START){
    		if(out == NULL){
    			 out = fopen("result.csv", "wb"); 
				 if(out == false) exit(1);
			}
    		
	        forward = getDF(); 	//get distance to front
			right = getDR(); 	//get distance to right 45 degree
	        left = getDL(); 	//get distance to left 45 degree
	        
			//boundary check
			if(forward.distance > 30.0f) forward.distance = 30.0f;
			if(right.distance > 30.0f) right.distance = 30.0f;
			if(left.distance > 30.0f) left.distance = 30.0f;
	        DF = forward.distance - CAR_RADIUS;
	        DR = right.distance - CAR_RADIUS;
	        DL = left.distance - CAR_RADIUS;
	        if(DF < 0.0f || DR < 0.0f || DL < 0.0f){
	        	thread_start_flag = THREAD_FINISH;
				sprintf(textstring, "Fail: Please try again...");
				SetWindowText(textField_info, textstring);
				continue;
			}
	        //NN
			float in[NUM_OF_INPUT_DIMENSION];
	        in[0] = DF;
	        in[1] = DR;
	        in[2] = DL;
			calculate_output(best_gene, in);
			
			//1~0 => 40~-40
			/*if(best_gene.F > 1.0f) best_gene.F = 1.0f;
			else if(best_gene.F < 0.0f) best_gene.F = 0.0f;
			best_gene.F = (best_gene.F) * 80.0f - 40.0f;*/
	        
	        theta = -best_gene.F;
	        
	        //degree to rad
	        theta_rad = theta/180.0f*PI;
	        degree_rad = degree/180.0f*PI;
	        
			//printf("R:%d, L:%d, (x,y)=(%.2f, %.2f), deg: %.2f, DR: %.2f, DL: %.2f, theta = %.2f\n", flagR, flagL, x, y, degree, DR, DL, theta);
	        
			//move to next point
	        x = x + cos(degree_rad + theta_rad) + sin(theta_rad) * sin(degree_rad);
	        y = y + sin(degree_rad + theta_rad) - sin(theta_rad) * cos(degree_rad);
	        degree = degree - asin(2.0f * sin(theta_rad) / car_length) / PI * 180.0f;
	        //printf("%.2f %.2f %.2f\n", asin(2.0f * sin(theta_rad) / car_length), sin(theta_rad), theta_rad);
			
			//write to csv file
			fprintf(out, "%.2f, %.2f\n", x, y);
			
			//arrive finish line
			if(y >= 37.0f) flag = 1;
			
			//display text (x, y), DR, DL
			sprintf(textstring, "(x, y)=(%.2f, %.2f),DF:%.2f,DR:%.2f,DL:%.2f", x, y, DF, DR, DL);
			SetWindowText(textField_info, textstring);
			plot(hwnd, x, y, degree, forward, right, left);
			
			//check
			if(flag){
				if(x >= 18 && x <= 30){
					sprintf(textstring, "AC: Complete the fuzzy simulation.");
					thread_start_flag = THREAD_FINISH;
				}
				else{
					thread_start_flag = THREAD_FINISH;
					sprintf(textstring, "Fail: Please try again...");
				}
				SetWindowText(textField_info, textstring);
			}
			
			//_sleep(500);
		}else if(thread_start_flag == THREAD_RESET){
			//nothing to do
			
			pthread_mutex_lock(&mutex); 
			//get input
			GetWindowText(textBox_x, buf, 20);
			tmp_x = (float)atoi(buf);
			GetWindowText(textBox_y, buf, 20);
			tmp_y = (float)atoi(buf);
			GetWindowText(textBox_deg, buf, 20);
			tmp_deg = (float)atoi(buf);
			init(tmp_x, tmp_y, tmp_deg);
			pthread_mutex_unlock(&mutex);
			flag = 0;
			locus_cnt = 0;
    		fclose(out);
    		out = NULL;
		}
		//_sleep(10);
		_sleep(10);
    }
    if(out != NULL)	fclose(out);
    
}

void* test_func(void* arg){	
	int i = 0;
	HWND hwnd;
	hwnd = FindWindow(_TEXT(fuClassName),_TEXT(fuWindowName));
	if(hwnd == NULL) exit(1);
	while(1){
		
		if(thread_start_flag == THREAD_START){//start
			// do something
		    main_thread(hwnd);
			
		}else if(thread_start_flag == THREAD_FINISH){//program finish
			//break;
		}else if(thread_start_flag == THREAD_RESET){//reset
  			//reset
		}else if(thread_start_flag == THREAD_STOP){
  		    //do nothing
		}
		_sleep(500);
	}
	pthread_exit(0);
}

