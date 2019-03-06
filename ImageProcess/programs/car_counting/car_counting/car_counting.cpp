#include "opencv2/opencv.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stack>
#include <vector>
//#define line_count_threshold 50 //投票箱大於該大小，才開始採納
#define line_min_adjacent_width 5 //最小相鄰
#define frame_range 90 //video height - frame_title_y - frame_range > 0 //由下往上數
//240 - 60 - 90 > 0
#define line_area_size 60 //線段大小
#define noise_class 50 //小於像素數量大小之空洞就不填補

using namespace std;
using namespace cv;

const int frame_title_y = 60; 

//實做在本cpp檔下面
void rgb_to_gray(Mat& in, Mat& out);
//理想高通濾波
void highpass_filter(Mat& in, Mat& out);
//泡沫排序
inline uchar bubble_sort(uchar* mask, const int mask_size);
//中值濾波
void median_filter(Mat& in, Mat& out, const int n);
//最小值濾波
void min_filter(Mat& in, Mat& out);
//sobel
void sobel(Mat& in, Mat& out);
//保留兩條線內影像
void split_two_lines(Mat& frame, int x1, int x2);
//畫兩條線
void draw_two_lines(Mat& frame, int x1, int x2);
//畫一條線
void draw_one_lines(Mat& frame, int x1);
//型態學
//3*3
//010
//111
//010
void dilation3x3(Mat& in, Mat& out);
void erosion3x3(Mat& in, Mat& out);
//先做erosion再做dilation
void openning(Mat& frame, Mat& out);
//先做dilation再做erosion
void closing(Mat& frame, Mat& out);
//影像相減 in1 - in2 => out
void diff_frame(Mat& in1, Mat& in2, Mat& out);
//線車輛計數
void car_counting_line(Mat& foreground_frame, int& car_count, int line_pos);
//區域車輛計數
void car_counting_area(Mat& foreground_frame, int& car_count, int area_start_pos, int area_size);
//清除文字
void clearText(Mat& foreground_frame);
//8連通
void connected_component8(uchar* image, int** map, int nr, int nc);
//填補空洞
void hole_filling(uchar* image, int** map, int nr, int nc);

int main(int args, char** argv)
{
	char in_file_dir[] = "C:\\Users\\MIAT\\Desktop\\影像處理\\104522065 張翔\\data\\test video\\2015-12-27 18-35.avi";
	//輸出影片路徑
	char out_file_dir[] = "C:\\Users\\MIAT\\Desktop\\影像處理\\104522065 張翔\\data\\output video\\2015-12-27 18-35.avi";
	char out_foreground_file_dir[] = "C:\\Users\\MIAT\\Desktop\\影像處理\\104522065 張翔\\data\\output video\\2015-12-27 18-35-foreground.avi";
	
	//測試影片路徑
	//VideoCapture inputVideo("C:\\Users\\MIAT\\Desktop\\影像處理\\104522065 張翔\\data\\test video\\2016-01-09 09-37.avi"); // 早上
	//VideoCapture inputVideo("C:\\Users\\MIAT\\Desktop\\影像處理\\104522065 張翔\\data\\test video\\2015-12-27 12-50.avi"); // 中午
    //VideoCapture inputVideo("C:\\Users\\MIAT\\Desktop\\影像處理\\104522065 張翔\\data\\test video\\2015-12-27 12-35.avi"); // 中午
    //VideoCapture inputVideo("C:\\Users\\MIAT\\Desktop\\影像處理\\104522065 張翔\\data\\test video\\2015-12-27 13-01.avi"); // 中午
    //VideoCapture inputVideo("C:\\Users\\MIAT\\Desktop\\影像處理\\104522065 張翔\\data\\test video\\2015-12-27 13-11.avi"); // 中午
    //VideoCapture inputVideo("C:\\Users\\MIAT\\Desktop\\影像處理\\104522065 張翔\\data\\test video\\2015-12-27 13-21.avi"); // 中午
	//VideoCapture inputVideo("C:\\Users\\MIAT\\Desktop\\影像處理\\104522065 張翔\\data\\test video\\2015-12-27 17-31.avi"); // 下午
	//VideoCapture inputVideo("C:\\Users\\MIAT\\Desktop\\影像處理\\104522065 張翔\\data\\test video\\2015-12-27 18-35.avi"); // 晚上
	
	VideoCapture inputVideo(in_file_dir);
	VideoWriter outputVideo, outputForegroundVideo;

	int ex = static_cast<int>(inputVideo.get(CV_CAP_PROP_FOURCC));     // Get Codec Type- Int form
    cv::Size S = cv::Size((int) inputVideo.get(CV_CAP_PROP_FRAME_WIDTH),    // Acquire input size
                  (int) inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT));

    outputVideo.open(out_file_dir, ex, inputVideo.get(CV_CAP_PROP_FPS), S, false);
    outputForegroundVideo.open(out_foreground_file_dir, ex, inputVideo.get(CV_CAP_PROP_FPS), S, false);
	
	// check read file
    if(!inputVideo.isOpened()){  
        printf("cannot read video\n");
		return -1;
	}
	//check write file
	if(!outputVideo.isOpened()){
		printf("cannot write video\n");
		return -1;
	}

	
    Mat gray_frame;					//輸入灰階影像
	Mat foreground_frame;			//前景影像
	Mat background_mean_frame;		//平均背景建模影像
	Mat foreground_highpass_frame;	//暫存影像

	//sobel
	Mat sobel_frame;				//輸入灰階影像做sobel
	Mat background_sobel_frame;		//背景影像做sobel
	Mat foreground_sobel_frame;		//前景影像做sobel
	Mat foreground_sobel_mor_frame;	//sobel前景影像做形態學

	char text_count[20];
	int frame_count = -1;
	int car_count = 0;
	const int nr = inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT);
	const int nc = inputVideo.get(CV_CAP_PROP_FRAME_WIDTH);
	int max;
	int frame_average = 0;
	int init_finish_flag = 0;
	int line_pos = nr >> 1;

	const double alpha = 0.05;
	const double beta = 0.5;

	//中值建模所需變數
	const int median_frame_count = 10; 
	const int median_frame_half_count = median_frame_count>>1; 
	Mat background_median_frame;
	Mat frame_arr[median_frame_count];
	uchar* frame_pointer[median_frame_count];
	int frame_arr_count = 0;
	uchar median_mask[median_frame_count] = {0};
	
	//動態測量線位置, dynamic_line_pos
	int* dynamic_line_count = (int*)malloc((nr - frame_title_y + 1) * sizeof(int));
	if(dynamic_line_count == NULL){ printf("memory allocation failed\n");return -1;}
	for(int i = 0, size = nr - frame_title_y;i < size;i++) dynamic_line_count[i] = 0;

	//video information
	printf("video height = %d, and width = %d\n", nr, nc);
	
	//
    namedWindow("gray",1);
    namedWindow("background_mean_frame",1);
    namedWindow("background_median_frame",1);
    namedWindow("foreground_frame",1);
	
	//第一列
	cv::moveWindow("gray", 0, 0);
	cv::moveWindow("background_mean_frame", nr * 1.5, 0);
	cv::moveWindow("foreground_frame", nr * 3, 0);
	
	//第二列
	cv::moveWindow("background_median_frame", nr * 1.5, nc * 0.75);
	
    Mat frame;
	
	int** map = (int**)calloc(nr, sizeof(int*));
	for(int i = 0;i < nr;i++)
		map[i] = (int*) calloc(nc, sizeof(int));

    for(;;)
    {
		double t = (double)cvGetTickCount();
		max = 0;
		frame_average = 0;
        inputVideo >> frame; // get a new frame from camera
        if(frame.empty()) break;
		if(++frame_count % 6 != 0) continue;
		cvtColor(frame, gray_frame, CV_RGB2GRAY);
		//rgb_to_gray(frame, gray_frame);
		
		memset(map, 0, sizeof(map));
		//初始化
		if(frame_count == 0){ 
			//sobel
			sobel_frame = Mat(nr, nc, CV_8UC1, Scalar(0));
			foreground_sobel_frame = Mat(nr, nc, CV_8UC1, Scalar(0));
			background_sobel_frame = Mat(nr, nc, CV_8UC1, Scalar(0));
			foreground_sobel_mor_frame = Mat(nr, nc, CV_8UC1, Scalar(0));
			
			//移動平均所產生之背景
			foreground_frame = Mat(nr, nc, CV_8UC1, Scalar(0));
			foreground_highpass_frame = Mat(nr, nc, CV_8UC1, Scalar(0));

			//將第一張影像視為背景
			background_mean_frame = gray_frame.clone();
			
			//將多影像中值濾波視為背景
			background_median_frame = Mat(nr, nc, CV_8UC1, Scalar(0));
		}
		
		
		//中值建模
		if(!init_finish_flag){
			frame_arr[frame_arr_count] = gray_frame.clone();
			frame_pointer[frame_arr_count] = (uchar*)frame_arr[frame_arr_count].data;

			if(frame_count/6 == median_frame_count - 1){
				int y_cols = gray_frame.cols * frame_title_y;
				uchar* bmf = (uchar*)background_median_frame.data;
				for(int y = frame_title_y;y < gray_frame.rows;y++){
					for(int x = 0;x < gray_frame.cols;x++){
						//get frame pixels of (i, j)
						for(int k = 0;k < median_frame_count;k++){
							median_mask[k] = *(frame_pointer[frame_arr_count] + y_cols + x);
						}
						//find median value
						*(bmf + y_cols + x) = bubble_sort(median_mask, median_frame_count);
					}
					y_cols += gray_frame.cols;
				}
				background_mean_frame = background_median_frame.clone();
				init_finish_flag = 1;
			}
			if(frame_arr_count == median_frame_count - 1)
				frame_arr_count = 0;
			else
				frame_arr_count++;
			
		}
		//更新背景

		uchar* bmf = (uchar*)background_mean_frame.data;
		uchar* gf = (uchar*)gray_frame.data;
		uchar* ff = (uchar*)foreground_frame.data;
		for (int y = frame_title_y, y_cols = frame.cols * frame_title_y; y < frame.rows - 1; ++y, y_cols += frame.cols) {
			for (int x = 0; x < frame.cols - 1; ++x) {
				int mean;
				uchar& frame1 = *(bmf + y_cols + x);
				uchar& frame2 = *(gf + y_cols + x);
				uchar& frame3 = *(ff + y_cols + x);

				//if(!init_finish_flag){
					mean = (int)frame1 * (1-alpha) + (int)frame2 * alpha;
					frame1 = (uchar)(mean);
				//}
				frame3 = abs((int)frame1 - (int)frame2);
				frame_average += frame2;
			}
		}
		//取得輸入影像平均灰階值
		frame_average = frame_average / ((frame.rows - 2) * (frame.cols - 2));
		
		//以輸入影像平均灰階值做二值化
		for (int y = frame_title_y; y < frame.rows - 1; ++y) {
			for (int x = 0; x < frame.cols - 1; ++x) {
				uchar& frame1 = foreground_frame.at<uchar>(y, x);
				if(foreground_frame.at<uchar>(y, x) < frame_average * beta) frame1 = 0;
				else frame1 = 255;
			}
		}
		
		
		ff = (uchar*)foreground_frame.data;
		connected_component8(ff, map, nr, nc);
		hole_filling(ff, map, nr, nc);

		//去除雜訊
		erode(foreground_frame, foreground_highpass_frame, Mat());
		dilate(foreground_highpass_frame, foreground_frame, Mat());
		dilate(foreground_frame, foreground_highpass_frame, Mat());
		dilate(foreground_highpass_frame, foreground_frame, Mat());
		dilate(foreground_frame, foreground_highpass_frame, Mat());
		erode(foreground_highpass_frame, foreground_frame, Mat());
		erode(foreground_frame, foreground_highpass_frame, Mat());
		erode(foreground_highpass_frame, foreground_frame, Mat());
		

		//統計frame_title_y以下前景像素值
		for (int y = frame_title_y; y < nr - frame_range; ++y) {
			int count = 0;
			for (int x = 0; x < frame.cols; ++x) {
				uchar& frame1 = foreground_frame.at<uchar>(y, x);
				if(frame1 != 0) count++;
				else count = 0;
				if(frame1 != 0 && count >= line_min_adjacent_width)	dynamic_line_count[y - frame_title_y] ++;
			}
		}
		//找出現最多像素的列
		max = 0;
		for (int y = 0, size = nr - frame_range; y < size; ++y) {
			if(max < dynamic_line_count[y])	{
				max = dynamic_line_count[y];
				line_pos = y + frame_title_y;
			}
		}
		printf("line position: %d\n", line_pos);

		//車輛計數
		//線段法
		//car_counting_line(foreground_frame, car_count, line_pos);
		//區域法
		car_counting_area(foreground_frame, car_count, line_pos, line_area_size);

		//清除文字
		clearText(foreground_frame);
		//填上文字
		sprintf(text_count, "%d", car_count); 
		//cv::putText((Mat)img, (char*)text, (Point)textOrg, fontFace, (double)fontScale, Scalar::all(255), thickness,8);
		cv::putText(foreground_frame, text_count, cvPoint(30,30), cv::FONT_HERSHEY_SCRIPT_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 2); 
		cv::putText(gray_frame, text_count, cvPoint(nc-60,25), cv::FONT_HERSHEY_SCRIPT_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 2); 

		//畫出測量車輛線
		draw_one_lines(foreground_frame, line_pos);
		draw_one_lines(gray_frame, line_pos);
		
		imshow("gray", gray_frame);
		imshow("background_mean_frame", background_mean_frame);
		imshow("foreground_frame", foreground_frame);
		imshow("background_median_frame", background_median_frame);

        if(waitKey(30) >= 0) break;
		t = (double)cvGetTickCount() - t;
		printf("frame number: %d, cost time: %.6f\n", frame_count, t / ((double)cvGetTickFrequency()*1000.));
		
		//人工計數用
		//_sleep(50);

		outputVideo.write(gray_frame);
		outputForegroundVideo.write(foreground_frame);
    }
	inputVideo.release();
	outputVideo.release();

	//釋放記憶體
	sobel_frame.release();
	foreground_sobel_frame.release();
	background_sobel_frame.release();
	foreground_sobel_mor_frame.release();
			
	//移動平均所產生之背景
	foreground_frame.release();
	foreground_highpass_frame.release();

	//將第一張影像視為背景
	background_mean_frame.release();
			
	//將多影像中值濾波視為背景
	background_median_frame.release();

	printf("car_count = %d\n", car_count);
	system("PAUSE");
    return 0;
}

void rgb_to_gray(Mat& in, Mat& out){
	Vec3b* pixel_in = (Vec3b*)in.data;
	uchar* pixel_out = (uchar*)out.data;
    for (int i = 0; i < in.rows; ++i) {
        for (int j = 0; j < in.cols; ++j) {
            uchar& out_pixel = *(pixel_out + i * in.cols + j);
			int tmp = (*(pixel_in + (i * in.cols + j)))[0] * 29 + //b
			(*(pixel_in + (i * in.cols + j)))[1] * 150 +  //g
			(*(pixel_in + (i * in.cols + j)))[2] * 77; //r
			//int tmp = (*(pixel_in + (i * in.cols + j)))[0] + (*(pixel_in + (i * in.cols + j)))[1] + (*(pixel_in + (i * in.cols + j)))[2];
			//tmp /= 3;
			tmp = tmp >> 8;
			out_pixel = (uchar)tmp;
			
        }
    }
}

//理想高通濾波
void highpass_filter(Mat& in, Mat& out){
	int sum;
	const float div = 1.0/9.0;
	int mask[3][3] = {{-1, -1, -1}, {-1, 8, -1}, {-1, -1, -1}};
	uchar* pixel_in = (uchar*)in.data;
	uchar* pixel_out = (uchar*)out.data;
    for (int i = 1; i < in.rows - 1; ++i) {
        for (int j = 1; j < in.cols - 1; ++j) {
            sum = 0;
            uchar& out_pixel = *(pixel_out + i * in.cols + j);
			for(int z = -1;z < 1;z++)
				for(int m = -1;m < 1;m++)
					sum += *(pixel_in + (i + z) * in.cols + j + m) * mask[z + 1][m + 1];
			sum = (int)(div * abs(sum));
			
			//if(sum > 48) out_pixel = 255;
			//else out_pixel = 0;	
			if(sum >= 255) out_pixel = 255;
			else if(sum <= 0) out_pixel = 0;
			else out_pixel = (uchar)sum;
        }
    }
}

//泡沫排序
inline uchar bubble_sort(uchar* mask, const int mask_size){
	int sort_round = mask_size - 1;
	int flag;
	uchar tmp;
	
	for(int i = 0;i < sort_round;i++){
		flag = 0;
		for(int j = 0;j < sort_round - i;j++){
			if(mask[j] > mask[j + 1]){
				tmp = mask[j];
				mask[j] = mask[j + 1];
				mask[j + 1] = tmp;
				flag = 1;
			}
		}
		if(flag == 0) break;
	}
	return mask[mask_size>>1];
}

//中值濾波
void median_filter(Mat& in, Mat& out, const int n){
	const int mask_size = n * n;
	const int n_2 = n >> 1;
	uchar* mask = (uchar*)calloc(mask_size, sizeof(uchar));
	if(mask == NULL){
		printf("memory allocation failed\n");
		exit(1);
	}

    for (int i = n_2, rows = in.rows - n_2; i < rows; ++i) {
        for (int j = n_2, cols = in.cols - n_2; j < cols; ++j) {
            uchar& out_pixel = out.at<uchar>(i, j);
			for(int z = -n_2;z <= n_2;z++)
				for(int m = -n_2;m <= n_2;m++)
					mask[(z + n_2) * 3 + m + n_2] = in.at<uchar>(i + z, j + m);

			out_pixel = bubble_sort(mask, mask_size);
        }
    }
	free(mask);
}

//最小值濾波
void min_filter(Mat& in, Mat& out){
	int min = 255;

    for (int i = 1; i < in.rows - 1; ++i) {
        for (int j = 1; j < in.cols - 1; ++j) {
			min = 255;
            uchar& out_pixel = out.at<uchar>(i, j);
			for(int z = -1;z <= 1;z++)
				for(int m = -1;m <= 1;m++)
					if(in.at<uchar>(i + z, j + m) < min)
						min = in.at<uchar>(i + z, j + m);

			out_pixel = min;
        }
    }
}

//sobel
void sobel(Mat& in, Mat& out){
	int hsum, vsum, sum;
	//int hmask[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
	//int vmask[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};

    for (int i = 1; i < in.rows - 1; ++i) {
        for (int j = 1; j < in.cols - 1; ++j) {
            uchar& out_pixel = out.at<uchar>(i, j);

			hsum = - (int)in.at<uchar>(i - 1, j - 1) - in.at<uchar>(i - 1, j) * 2 - in.at<uchar>(i - 1, j + 1)
				   + in.at<uchar>(i + 1, j - 1) + in.at<uchar>(i + 1, j) * 2 + in.at<uchar>(i + 1, j + 1);
			
			vsum = - (int)in.at<uchar>(i - 1, j - 1) - in.at<uchar>(i, j - 1) * 2 - in.at<uchar>(i + 1, j - 1)
				   + in.at<uchar>(i - 1, j + 1) + in.at<uchar>(i, j + 1) * 2 + in.at<uchar>(i + 1, j + 1);

			sum = abs(hsum) + abs(vsum);
			if(sum >= 255) out_pixel = 255;
			else if(sum <= 0) out_pixel = 0;
			else out_pixel = (uchar)sum;
        }
    }
}

//保留兩條線內影像
void split_two_lines(Mat& frame, int x1, int x2){
	
    for (int i = 0; i < frame.rows; ++i) {
        for (int j = 0; j < frame.cols; ++j) {
			if(i <= x1 || i >= x2){
				uchar& out_pixel = frame.at<uchar>(i, j);
				out_pixel = 0;
			}
        }
    }

}

//畫兩條線
void draw_two_lines(Mat& frame, int x1, int x2){
	
    for (int j = 0; j < frame.cols; ++j) {
		uchar& out_pixel1 = frame.at<uchar>(x1, j);
		uchar& out_pixel2 = frame.at<uchar>(x2, j);
		out_pixel1 = out_pixel2 = 255;
    }
}

//畫一條線
void draw_one_lines(Mat& frame, int x1){
	
    for (int j = 0; j < frame.cols; ++j) {
		uchar& out_pixel1 = frame.at<uchar>(x1, j);
		out_pixel1 = 255;
    }
}


//3*3
//010
//111
//010
void dilation3x3(Mat& in, Mat& out){
	int y, x, i, j;
	int rows = in.rows - 1;
	int cols = in.cols - 1;
	int y_cols = 0;
	int flag = 0;
	uchar* pin = (uchar*) (in.data);
	uchar* pout = (uchar*) (out.data);
	
	for(y = 0;y < rows;y++){
		*(pout + y * in.cols) = 0;
		*(pout + y * in.cols + cols) = 0;
	}
	for(x = 0;x < in.cols;x++){
		*(pout + x) = 0;
		*(pout + rows * in.cols + x) = 0;
	}
	
	for(y = 1;y < rows;y++){
		y_cols += in.cols;
		for(x = 1;x < cols;x++){
			flag = 0;
			for(i = y_cols - in.cols;i <= y_cols + in.cols;i += in.cols){
				for(j = -1;j <= 1;j++){
					if(*(pin + i + x + j)){
						flag = 1;
						i = (y_cols + (in.cols << 1));
						j = 2;
					}
				}
			}
			if(flag) *(pout + y_cols + x) = 255;
			else *(pout + y_cols + x) = 0;
		}
	}
}

void erosion3x3(Mat& in, Mat& out){
	int y, x, i, j;
	int rows = in.rows - 1;
	int cols = in.cols - 1;
	int y_cols = 0;
	int flag = 0;
	uchar* pin = (uchar*) (in.data);
	uchar* pout = (uchar*) (out.data);
	
	for(y = 0;y < rows;y++){
		*(pout + y * in.cols) = 0;
		*(pout + y * in.cols + cols) = 0;
	}
	for(x = 0;x < in.cols;x++){
		*(pout + x) = 0;
		*(pout + rows * in.cols + x) = 0;
	}
	for(y = 1;y < rows;y++){
		y_cols += in.cols;
		for(x = 1;x < cols;x++){
			flag = 1;
			for(i = y_cols - in.cols;i <= y_cols + in.cols;i += in.cols){
				for(j = -1;j <= 1;j++){
					if(*(pin + i + x + j) == 0){
						flag = 0;
					}
				}
			}
			if(flag) *(pout + y_cols + x) = 255;
			else *(pout + y_cols + x) = 0;
		}
	}
}
//先做erosion再做dilation
void openning(Mat& frame, Mat& out){
	Mat tmp;
	erode(frame, tmp, Mat());
	dilate(tmp, out, Mat());
	tmp.release();
}

//先做dilation再做erosion
void closing(Mat& frame, Mat& out){
	Mat tmp;
	dilate(frame, tmp, Mat());
	erode(tmp, out, Mat());
	tmp.release();
}

//影像相減 in1 - in2 => out
void diff_frame(Mat& in1, Mat& in2, Mat& out){
	int diff;
	for (int y = frame_title_y+1; y < in1.rows; ++y) {
		for (int x = 0; x < in1.cols; ++x) {
			diff = (int)in1.at<uchar>(y, x);
			diff = diff - in2.at<uchar>(y, x);
			uchar& frame1 = out.at<uchar>(y, x);
				
			diff = abs(diff);
			frame1 = (uchar)diff;
			//if(diff > frame_average) frame1 = mean;
		}
	}
}

//線車輛計數
void car_counting_line(Mat& foreground_frame, int& car_count, int line_pos){
	//const int obj_min_pixel = 5;
	//int* obj = (int*) calloc(foreground_frame.cols/obj_min_pixel, sizeof(int));
	static int now_count;
	static int prev_count;
	int flag = 0;
	now_count = 0;
	for (int x = 0; x < foreground_frame.cols; ++x) {
		uchar& frame1 = foreground_frame.at<uchar>(line_pos, x);
		if(frame1){
			flag = 1;
		}else{
			if(flag == 1) now_count++;
			flag = 0;
		}
	}
	if(prev_count > now_count)
		car_count += prev_count - now_count;
	prev_count = now_count;
	printf("%d %d\n", prev_count, now_count);
}

//區域車輛計數
void car_counting_area(Mat& foreground_frame, int& car_count, int area_start_pos, int area_size){
	//const int obj_min_pixel = 5;
	//int* obj = (int*) calloc(foreground_frame.cols/obj_min_pixel, sizeof(int));
	static int now_count;
	static int prev_count;
	int flag = 0, area_flag = 0, adj_count = 0;
	now_count = 0;
	//防呆 當區域大於等於影像最大高度，遞減
	while(area_start_pos + area_size >= foreground_frame.rows) area_size--;

	for (int x = 0; x < foreground_frame.cols; ++x) {
		area_flag = 0;
		for(int y = area_start_pos, area_end_pos = area_start_pos + area_size;y < area_end_pos;y++){
			uchar& frame1 = foreground_frame.at<uchar>(area_start_pos, x);
			if(frame1){
				area_flag++;
			}
		}
		if(area_flag > line_min_adjacent_width){
			flag++;
		}
		else{
			if(flag > line_min_adjacent_width) now_count++;
			flag = 0;
		}
	}
	if(prev_count > now_count)
		car_count += prev_count - now_count;
	prev_count = now_count;
	printf("%d %d\n", prev_count, now_count);
}

void clearText(Mat& foreground_frame){
	for (int y = 0; y < frame_title_y; ++y)
		for(int x = 0;x < foreground_frame.cols;x++)
			foreground_frame.at<uchar>(y, x) = 0;
}

//8連通
void connected_component8(uchar* image, int** map, int nr, int nc){
	int y, x, i, j, k;
	int class_min_number;
	int class_count;
	int new_class = 1;
	vector<int> equal_class;
	stack<int> stmp;
	int qx, qy, sx;
	equal_class.push_back(0);
	
	//first pass
	for(y = frame_title_y;y < nr - 1;y++){
		for(x = 1;x < nc - 1;x++){
			if(*(image + y * nc + x) == 0){
				map[y][x] = 0;
			}else{
				class_min_number = new_class+1;
				class_count = 0;
				qx = x - 1;
				qy = y - 1;
				sx = x + 1;
				//q r s
				//x p
				
				//q
				if(map[qy][qx] && *(image + qy * nc + qx) && class_min_number > map[qy][qx]){
					class_min_number = map[qy][qx];
					class_count++;
				}
				//r
				if(map[qy][x] && *(image + qy * nc + x) && class_min_number > map[qy][x]){
					class_min_number = map[qy][x];
					class_count++;
				}
				//s
				if(map[qy][sx] && *(image + qy * nc + sx) && class_min_number > map[qy][sx]){
					class_min_number = map[qy][sx];
					class_count++;
				}
				//t
				if(map[y][qx] && *(image + y * nc + qx) && class_min_number > map[y][qx]){
					class_min_number = map[y][qx];
					class_count++;
				}
				//labeling
				//store equivalent class
				if(class_count == 0){
					equal_class.push_back(new_class);
					map[y][x] = new_class++;
				} 
				else{
					//printf("%d", class_min_number);
					if(map[qy][qx] && equal_class[map[qy][qx]] > class_min_number)
						equal_class[map[qy][qx]] = class_min_number;
					if(map[qy][x] && equal_class[map[qy][x]] > class_min_number) 
						equal_class[map[qy][x]] = class_min_number;
					if(map[qy][sx] && equal_class[map[qy][sx]] > class_min_number) 
						equal_class[map[qy][sx]] = class_min_number;
					if(map[y][qx] && equal_class[map[y][qx]] > class_min_number) 
						equal_class[map[y][qx]] = class_min_number;
					map[y][x] = class_min_number;
				}
			}
		}
	}
	int* equal_class_array = (int*)calloc(equal_class.size(), sizeof(int));
	int* equal_class_count = (int*)calloc(equal_class.size(), sizeof(int));
	int* walked = (int*)calloc(equal_class.size(), sizeof(int));
	vector<int>::iterator it;
	// 尋訪
	for(i = 0,it=equal_class.begin(); it!=equal_class.end(); it++ , i++) 
		equal_class_array[i] =  *it; 
	 
	//for(i = 0;i < equal_class.size();i++)
	//	printf("%d, ", array[i]);
	//printf("\n");
	for(i = 0;i < equal_class.size();i++){
		k = i;
		while(equal_class_array[k] != k && !walked[k])
			stmp.push(k), walked[k] = 1, k = equal_class_array[k];
	
		while(!stmp.empty())
			j = stmp.top(), stmp.pop(), equal_class_array[j] = k;
	}
	 
	equal_class_array[0] = 0;
	
	//for(i = 0;i < equal_class.size();i++)
	//	printf("%d, ", array[i]);
	
	//second pass
	for(y = frame_title_y;y < nr - 1;y++){
		for(x = 1;x < nc - 1;x++){
			equal_class_count[equal_class_array[map[y][x]]]++;
		}
	}
	for(i = 0;i < equal_class.size();i++)
		if(equal_class_count[i] < noise_class){
			//printf("equal_class_count[%d] = %d\n", i, equal_class_count[i]);
			equal_class_count[i] = 0;
		}//else
			//printf("equal_class_count[%d] = %d\n", i, equal_class_count[i]);

	for(y = frame_title_y;y < nr - 1;y++){
		for(x = 1;x < nc - 1;x++){
			int& tmp = map[y][x];
			if(equal_class_count[tmp] != 0) tmp = equal_class_array[tmp];
			else tmp = 0;
		}
	}
	
	equal_class.clear();
	free(equal_class_array);
	free(walked);
}
//填補空洞
void hole_filling(uchar* image, int** map, int nr, int nc){
	int y, x, z;
	int start, end, obj_prev, obj_next;
	for(y = 1;y < nr - 1;y++){
		start = 1;
		end = 2;
		for(x = 1;x < nc - 1;x++){
			while(start < nc - 2){
				obj_prev = map[y][start];
				if(!obj_prev){
					start++;
					end++; 
				}else
					break;
			}
			while(end < nc - 1){
				obj_next = map[y][end];
				if(!obj_next){
					end++;
				}else if(obj_next == obj_prev){
					//hole filling between obj_prev and obj_next
					for(z = start;z < end;z++)
						*(image + y * nc + z) = 255;
					start = end;
					end = start + 1;
					x = end;
				}else{
					start = end;
					end = start + 1;
					x = end;
					break;
				}
			}
		}
	}
	//vertical
	for(x = 1;x < nc - 1;x++){
		start = 1;
		end = 2;
		for(y = 1;y < nr - 1;y++){
			while(start < nr - 2){
				obj_prev = map[start][x];
				if(!obj_prev){
					start++;
					end++; 
				}else
					break;
			}
			while(end < nr - 1){
				obj_next = map[end][x];
				if(!obj_next){
					end++;
				}else if(obj_next == obj_prev){
					//hole filling between obj_prev and obj_next
					for(z = start;z < end;z++)
						*(image + z * nc + x) = 255;
					start = end;
					end = start + 1;
					y = end;
				}else{
					start = end;
					end = start + 1;
					y = end;
					break;
				}
			}
		}
	}

}
