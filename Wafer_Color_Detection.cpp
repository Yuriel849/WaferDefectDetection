//RGB -> HSV 변환(HUE 검출) 주희

#include "Common.h"
#include <opencv2/opencv.hpp>

std::string image_path = "../KCCImageNet/project_wafer/Donut_navy.png";


void Wafer_color(string& filePath)		//wafer 색상 검출할 이미지
{
	cv::Mat rgb_image = cv::imread(filePath, cv::IMREAD_COLOR);

	cv::Mat hsv_image;
	cv::cvtColor(rgb_image, hsv_image, cv::COLOR_BGR2HSV);		// RGB -> HSV 변환

	cv::Scalar mean_hsv = cv::mean(hsv_image);

	double average_hue = mean_hsv[0];
	std::cout << "hue 값: " << average_hue << std::endl;

	Point ptTxt = Point(50, 80);
	string msg = std::format("Hue = {:.1f}", average_hue);
	putText(hsv_image, msg, Point(ptTxt.x, ptTxt.y), FONT_HERSHEY_SIMPLEX, 2, CV_RGB(0, 255, 255), 4, 8);

	int stop = 0;
}


int main()
{

	Wafer_color(image_path);		// 함수 호출

	int a = 0;
}
