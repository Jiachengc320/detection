#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include<vector>
#include<math.h>
using namespace std;
using namespace cv;

typedef struct Input {
	/*
	输入结构体
	x,y,w,h:框信息:
	Degree_of_longitude,latitude:经纬度
	classes: 检测的类型
	image: 图像
	*/
	Input() {}
	Input(double x, double y, double w, double h, double Degree_of_longitude, double latitude, int classes,Mat image) { 
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
		this->Degree_of_longitude = Degree_of_longitude;
		this->latitude = latitude;
		this->classes = classes;
		this->image = image;
	}

	double x, y, w, h;
	double Degree_of_longitude, latitude;	
	int classes;			
	Mat image;
}OBJECT_INFORMATION;


double calculate_iou(Input input1,Input input2) {
	double x1 = input1.x;
	double y1 = input1.y;
	double w1 = input1.w;
	double h1 = input1.h;

	double x2 = input2.x;
	double y2 = input2.y;
	double w2 = input2.w;
	double h2 = input2.h;

	double x_1 = max(x1, x2);   
	double y_1 = max(y1, y2);    
	double x_2 = min(x1 + w1, x2 + w2);   
	double y_2 = min(y1 + h1, y2 + h2);   
	double intersectionArea = 1.0*max(0.0, x_2 - x_1) * max(0.0, y_2 - y_1);
	
	double box1Area = w1 * h1*1.0;
	double box2Area = w2 * h2*1.0;

	double iou = intersectionArea *1.0 / (box1Area + box2Area - intersectionArea);
	return iou;
}

double CalculateImageSimilarity(const cv::Mat& image1, const cv::Mat& image2) {


	Size targetSize(512, 512);
	Mat image1_resize,image2_resize;
	resize(image1, image1_resize, targetSize);
	resize(image2, image2_resize, targetSize);

	cv::Mat diff;
	cv::absdiff(image1_resize, image2_resize, diff);

	cv::Mat diffGray;
	cv::cvtColor(diff, diffGray, cv::COLOR_BGR2GRAY);

	cv::Scalar sumDiff = cv::sum(diffGray);
	double totalDiff = sumDiff[0] / (diffGray.cols * diffGray.rows * 255.0);

	double similarity = 1.0 - totalDiff;

	return similarity;
}


bool Judge(Input input1,Input input2) {
	/*
	判断是否发生变化
	true：发生变化
	false：未发生变化
	*/

	double Degree_of_longitude1 = input1.Degree_of_longitude;
	double Degree_of_longitude2 = input2.Degree_of_longitude;
	double latitude1 = input1.latitude;
	double latitude2 = input2.latitude;

	if (Degree_of_longitude1 != Degree_of_longitude2 || latitude1 != latitude2)
		return true;

	if (input1.classes != input2.classes)
		return true;

	double iou = calculate_iou(input1, input2);
	if (iou <= 0.8)  return true;
	cout << CalculateImageSimilarity(input1.image, input2.image) << endl;
	if (CalculateImageSimilarity(input1.image, input2.image) <= 0.95) return true;

	return false;

}


int main()
{
	// 示例
	Mat image1 = imread("1.jpg");
	Mat image2 = imread("2.jpg");

	Input input1(10, 10, 20, 30, 100, 100, 0, image1);
	Input input2(11, 10, 20, 30, 100, 100, 0, image2);
	cout << Judge(input1,input2) << endl;
}
