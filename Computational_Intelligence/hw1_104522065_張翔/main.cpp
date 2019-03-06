#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <unistd.h>
#include <pthread.h>
#include <conio.h>
#include <dos.h>
#include <math.h> 
#include "CCanvas.h"

//display
#define WINDOWS_WIDTH 640
#define WINDOWS_HEIGHT 480
#define OFFSET_X 200
#define OFFSET_Y 400
#define CAR_R 60
#define BLACK 0x000000
#define WHITE 0xffffff

//fuzzifier
#define NOT_VALID 45.0f
#define CAR_RADIUS 3.0f
#define NUM_OF_RULES 4
#define max(a, b) ((a > b) ? a : b)
#define min(a, b) ((a <= b) ? a : b)
#define car_length (CAR_RADIUS * 2.0f)
#define PI 3.14159265359f

float x, y;
float DR, DL, degree;
float theta;
int flag = 0;
int flagR, flagL; //which line is intersected

char textX[] = {"x"};
char textY[] = {"y"};
char textDeg[] = {"deg"};
char Arial[] = {"Arial"};

//thread
#define BUFFER_LEN 256
enum THREAD_STATE{
	THREAD_RESET, THREAD_START, THREAD_STOP, THREAD_FINISH
};
/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
char fuClassName[ ] = "fuzzy";
char fuWindowName[ ] = "Fuzzy simulation";
char textSave[20] = {""};
HWND btn_reset, btn_start, btn_stop, textField_x, textField_y, textField_deg, textField_info;
HWND textBox_x, textBox_y, textBox_deg;
THREAD_STATE thread_start_flag;
// execute thread
pthread_t thread_a;
pthread_mutex_t mutex;


float point2point(float x1, float y1, float x2, float y2){
	return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

/* six line */
float getDR(){
	flagR = 0;
	const float m = tan((degree - 45.0f)/180.0f*PI);
    const float b = y - m * x;
    float d1 = NOT_VALID, d3 = NOT_VALID, d4 = NOT_VALID, d5 = NOT_VALID;
    float y1, x3, x4, y5;
    float temp;
    y1 = m * 6.0f + b;
    x3 = (10.0f - b) / m;
    x4 = (22.0f - b) / m;
    y5 = m * 30.0f + b;
    
    if(y1 >= 0.0f && y1 <= 10.0f) d1 = point2point(6.0f, y1, x, y);
    if(x3 >= 6.0f && x3 <= 30.0f && x <= x3) d3 = point2point(x3, 10.0f, x, y);
    if(x4 >= -6.0f && x4 <= 18.0f && x <= x3) d4 = point2point(x4, 22.0f, x, y);
    if(y5 >= 10.0f && y5 <= 37.0f + 10.0f) d5 = point2point(30.0f, y5, x, y);
    
    temp = min(min(d1, d3), min(d4, d5));
    //which line is intersection with (x, y)
    if(temp == d1) flagR = 1;
    else if(temp == d3) flagR = 3;
    else if(temp == d4) flagR = 4;
    else if(temp == d5) flagR = 5;
    
	return temp;
}

float getDL(){
	flagL = 0;
	const float m = tan((degree + 45.0f)/180.0f*PI);
    const float b = y - m * x;
    float d2 = NOT_VALID, d4 = NOT_VALID, d5 = NOT_VALID, d6 = NOT_VALID;
    float y2, x4, y5, y6;
    float temp;
    y2 = m * (-6.0f) + b;
    x4 = (22.0f - b) / m;
    y5 = m * 30.0f + b;
    y6 = m * 18.0f + b;
    
    if(y2 >= 0.0f && y2 <= 22.0f) d2 = point2point(-6.0f, y2, x, y);
    if(x4 >= -6.0f && x4 <= 18.0f) d4 = point2point(x4, 22.0f, x, y);
	if(y5 >= 10.0f && y5 <= 37.0f + 30.0f) d5 = point2point(30.0f, y5, x, y);
    if(y6 >= 22.0f && y6 <= 37.0f + 10.0f) d6 = point2point(18.0f, y6, x, y);
    
    temp = min(min(d2, d4), min(d5, d6));
    if(temp == d2) flagL = 2;
    else if(temp == d4) flagL = 4;
    else if(temp == d5) flagL = 5;
    else if(temp == d6) flagL = 6;
    
	return temp;
} 

void init(float tmpx, float tmpy, float tmp_deg){
    x = tmpx;
	y = tmpy;
    DR = DL = 0.0f;
    degree = tmp_deg;
	theta = 0.0f;    
}
//DR, DL degree function
float positive(float x){
	const float p1 = 3.0f;
	const float p2 = 4.0f;
	const float p3 = 45.0f;
	if(p1 <= x && x <= p2) return x + -3.0f;
	else if(p2 <= x && x <= p3) return 1.0f;
	else return 0.0f;
}

float medium(float x){
	const float p1 = -4.0f;
	const float p2 = -3.0f;
	const float p3 = 3.0f;
	const float p4 = 4.0f;
	if(p1 <= x && x <= p2) return x + 4.0f;
	else if(p2 <= x && x <= p3) return 1.0f;
	else if(p3 <= x && x <= p4) return -x - 4.0f;
	else return 0.0f;
}

float negative(float x){
	const float p1 = -45.0f;
	const float p2 = -4.0f;
	const float p3 = -3.0f;
	if(p1 <= x && x <= p2) return 1.0f;
	else if(p2 <= x && x <= p3) return -x - 3.0f;
	else return 0.0f;
}
//theta output function
float left(float x){
	const float p1 = -40.0f;
	const float p2 = -8.0f;
	const float p3 = -4.0f;
	if(p1 <= x && x <= p2) return 1.0f;
	else if(p2 <= x && x <= p3) return -0.25f * x - 1.0f;
	else return 0.0f;
}

float forward(float x){
	const float p1 = -8.0f;
	const float p2 = -4.0f;
	const float p3 = 4.0f;
	const float p4 = 8.0f;
	if(p1 <= x && x <= p2) return 0.25f * x + 2.0f;
	else if(p2 <= x && x <= p3) return 1.0f;
	else if(p3 <= x && x <= p4) return -0.25f * x + 2.0f;
	else return 0.0f;
}

float right(float x){
	const float p1 = 4.0f;
	const float p2 = 8.0f;
	const float p3 = 40.0f;
	if(p1 <= x && x <= p2) return 0.25f * x - 1.0f;
	else if(p2 <= x && x <= p3) return 1.0f;
	else return 0.0f;
}

float fuzzy_inference(float DR_DL, int rule){
    //input: near, medium, far
    //output: left, forward, right
    
    //DR, DL代入input, get two firing strength
	//compare two value of firing strength, use minimum one
	//if there is three rules, will have three graphs, defuzzy then
    if(rule == 0)
		return positive(DR_DL);
	else if(rule == 1)
		return negative(DR_DL);
	else if(rule == 2)
		return medium(DR_DL);
	return 0.0f;
}

float defuzzifier(float firing_strength[3]){
	int i;
	float output = 0.0f, sum = 0.0, temp, r, l, f;
	//聯集部分取最大
	for(i = -40;i < -8;i++){
		temp = left((float)i) * firing_strength[1];
		output += temp * (float)i;
		sum += temp;
	}
	for(i = -8;i < -4;i++){
		temp = max(forward((float)i), left((float)i)) * min(firing_strength[2], firing_strength[1]);
		output += temp * (float)i;
		sum += temp;
	}
	for(i = -4;i < 4;i++){
		temp = forward((float)i) * firing_strength[2];
		output += temp * (float)i;
		sum += temp;
	}
	for(i = 4;i < 8;i++){
		temp = max(forward((float)i), right((float)i)) * min(firing_strength[2], firing_strength[0]);
		output += temp * (float)i;
		sum += temp;
	}
	for(i = 8;i <= 40;i++){
		temp = right((float)i) * (float)i * firing_strength[0];
		output += temp * (float)i;
		sum += temp;
	}
	
    return (sum == 0.0f) ? 0.0f : output/sum;
}

float fuzzifier(float DR_DL){
    float firing_strength[NUM_OF_RULES], result;
    int i;
    for(i = 0;i < NUM_OF_RULES;i++)
		firing_strength[i] = fuzzy_inference(DR_DL, i);
    result = defuzzifier(firing_strength);
    return result;
}

void plot(HWND &hwnd, float tmp_x, float tmp_y, float deg){
    HDC hdc;
    PAINTSTRUCT ps;
    CCanvas canvas;
    
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
    canvas.line(OFFSET_X + 6 * 10, OFFSET_Y, OFFSET_X + 6 * 10, OFFSET_Y - 10 * 10, 0x0000ff, 3);//1
    canvas.line(OFFSET_X - 6 * 10, OFFSET_Y, OFFSET_X - 6 * 10, OFFSET_Y - 22 * 10, 0x0000ff, 3);//2
    canvas.line(OFFSET_X + 6 * 10, OFFSET_Y - 10 * 10, OFFSET_X + 30 * 10, OFFSET_Y - 10 * 10, 0x0000ff, 3);//3
    canvas.line(OFFSET_X - 6 * 10, OFFSET_Y - 22 * 10, OFFSET_X + 18 * 10, OFFSET_Y - 22 * 10, 0x0000ff, 3);//4
    canvas.line(OFFSET_X + 30 * 10, OFFSET_Y - 10 * 10, OFFSET_X + 30 * 10, OFFSET_Y - 37 * 10, 0x0000ff, 3);//5
    canvas.line(OFFSET_X + 18 * 10, OFFSET_Y - 22 * 10, OFFSET_X + 18 * 10, OFFSET_Y - 37 * 10, 0x0000ff, 3);//6
    
    //end point
    canvas.ellipse(OFFSET_X + 24 * 10, OFFSET_Y - 37 * 10, CAR_R/4, CAR_R/4, 0x000000, 0x0000FF, 3);
    
    //draw car
	canvas.ellipse(OFFSET_X + (int)(tmp_x * 10.0f), OFFSET_Y - (int)(tmp_y * 10.0f), CAR_R, CAR_R, 0x000000, 0x0000FF, 3);
    canvas.line(OFFSET_X + (int)(tmp_x * 10.0f), OFFSET_Y - (int)(tmp_y * 10.0f), OFFSET_X + (int)(tmp_x * 10.0f) + (int)((float)CAR_R * cos(deg/180.0f*PI)), OFFSET_Y - (int)(tmp_y * 10.0f) - (int)((float)CAR_R * 1.25 * sin(deg/180.0f*PI)), 0xffffff, 3);//6

    
    // End application-specific layout section.
    //EndPaint(hwnd, &ps);
    ReleaseDC(hwnd, hdc);
}

int isNumber(char* str){
	int len = strlen(str), i, num;
	char* t = str;
	for(i=0;i < len;i++, t++){
		num = (int)*t;
		if(num < 0x30 || num > 0x39)
			return false;
	}
	return true;
}


void main_thread(HWND &hwnd){
	FILE* out = fopen("result.csv", "wb"); 
	if(out == NULL) exit(1);
	//write result to excel file
	char textstring[50];
	float theta_rad, degree_rad, tmp_x, tmp_y, tmp_deg;
	while(!flag){
    	if(thread_start_flag == THREAD_START){
	        DR = getDR() - CAR_RADIUS; //get distance to right 45 degree, and minus radius
	        DL = getDL() - CAR_RADIUS; //get distance to left 45 degree, and minus radius
	        
	        theta = fuzzifier(DR - DL);
	        
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
			sprintf(textstring, "(x, y)=(%.2f, %.2f), DR:%.2f, DL:%.2f", x, y, DR, DL);
			SetWindowText(textField_info, textstring);
			plot(hwnd, x, y, degree);
			_sleep(500);
		}else if(thread_start_flag == THREAD_RESET){
			break;
		}
		_sleep(500);
    }
	if(x >= 18 && x <= 30){
		sprintf(textstring, "AC: Complete the fuzzy simulation.");
		SetWindowText(textField_info, textstring);
		thread_start_flag = THREAD_FINISH;
	}
	else{
		sprintf(textstring, "Fail: Please try again...");
	} 
	
	SetWindowText(textField_info, textstring);
    fclose(out);
}

void* test_func(void* arg)
{	
	int i = 0;
	float tmp_x, tmp_y, tmp_deg;
	char buf[20];
	HWND hwnd;
	hwnd = FindWindow(_TEXT(fuClassName),_TEXT(fuWindowName));
	if(hwnd == NULL) exit(1);
	while(1){
		if(thread_start_flag == THREAD_START){//start
			// do something
			
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
    switch (message){                  /* handle the messages */
        case WM_CREATE:
        	//WS_CHILD: whether it is inside parent window
			 
            textField_info = CreateWindow("STATIC",
                                     "",
                                     WS_VISIBLE | WS_CHILD,
                                     5, 90, 300, 20,
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
			btn_reset = CreateWindow("BUTTON", //name
                                  "reset",//text
                                  WS_VISIBLE | WS_CHILD | WS_BORDER | BS_PUSHBUTTON | WS_TABSTOP,
                                  5, 60, 40, 20, //x, y, w, h
                                  hwnd, (HMENU) 1, NULL, NULL); //signal number
			btn_start = CreateWindow("BUTTON", //name
                                  "start",//text
                                  WS_VISIBLE | WS_CHILD | WS_BORDER | BS_PUSHBUTTON | WS_TABSTOP ,
                                  50, 60, 40, 20, //x, y, w, h
                                  hwnd, (HMENU) 2, NULL, NULL); //signal number
			btn_stop = CreateWindow("BUTTON", //name
                                  "stop",//text
                                  WS_VISIBLE | WS_CHILD | WS_BORDER | BS_PUSHBUTTON | WS_TABSTOP,
                                  100, 60, 40, 20, //x, y, w, h
                                  hwnd, (HMENU) 3, NULL, NULL); //signal number*/
			
			pthread_mutex_lock(&mutex); 
			SetFocus(textBox_x); //A handle to the window that will receive the keyboard input. If this parameter is NULL, keystrokes are ignored.
			EnableWindow(btn_start, TRUE);
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
					SetWindowText(textBox_x, "0");
					SetWindowText(textBox_y, "0");
					SetWindowText(textBox_deg, "90");
					EnableWindow(btn_start, TRUE);
					EnableWindow(btn_stop, FALSE);
					thread_start_flag = THREAD_RESET;
					pthread_mutex_unlock(&mutex);
					
					//MessageBox(handle control, content, windows name, button type);
					//MessageBox(hwnd, textSave, "Button", MB_OK);
					
				break;
				case 2://start
					//error beep sound
					
					MessageBeep(MB_ICONERROR);
					pthread_mutex_lock(&mutex); 
					EnableWindow(btn_start, FALSE);
					EnableWindow(btn_stop, TRUE);
					thread_start_flag = THREAD_START;
					pthread_mutex_unlock(&mutex);
					
					//do stuff here
					//create a thread to update screen, and go to next position
					//when stop button was pressed, stop thread
					
				break;
				case 3://stop
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

