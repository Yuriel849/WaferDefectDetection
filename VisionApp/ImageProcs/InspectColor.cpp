#include "Inspectcolor.h"



InspectColor::InspectColor()
{
}

InspectColor::~InspectColor()
{
}

void InspectColor::DetectColor(cv::Mat& draw_img)		//wafer 색상 검출할 이미지	
{
	cv::Mat gray, bin;
	cv::cvtColor(draw_img, gray, cv::COLOR_BGR2GRAY);

	// binary
	cv::threshold(gray, bin, 230, 250, cv::THRESH_BINARY_INV);

	// close
	int kernelSz = 4;
	int shape = cv::MorphShapes::MORPH_RECT;
	cv::Size sz = cv::Size(2 * kernelSz + 3, 2 * kernelSz + 2);
	cv::Mat SE = cv::getStructuringElement(shape, sz);
	cv::Mat close;
	int type = cv::MorphTypes::MORPH_CLOSE;
	cv::morphologyEx(bin, close, type, SE);

	cv::RNG rng(12345);
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(close, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

	// Find contour maximum area
	size_t maxContourIndex = 0;
	double maxContourArea = 0;

	for (size_t i = 0; i < contours.size(); i++)
	{
		double area = cv::contourArea(contours[i]);
		if (area > maxContourArea)
		{
			maxContourArea = area;
			maxContourIndex = i;
		}
	}

	cv::Mat contourMask = cv::Mat::zeros(close.size(), CV_8UC1);	//mask with only the selected contour filled
	cv::drawContours(contourMask, contours, static_cast<int>(maxContourIndex), cv::Scalar(255), cv::FILLED);

	// Apply the mask to the original image
	cv::Mat circleImage;
	draw_img.copyTo(circleImage, contourMask);

	cv::Mat hsv_image;
	cv::cvtColor(circleImage, hsv_image, cv::COLOR_BGR2HSV);	//HSV 변환

	std::vector<cv::Mat> channels;
	cv::split(hsv_image, channels);
	cv::Mat hue_channel = channels[0];	// Hue 채널 가져오기

	double mean_H = cv::mean(hue_channel)[0];	//평균 Hue(색조)
	std::cout << "Mean Hue : " << mean_H << std::endl;
	cv::putText(hsv_image, "Mean H : " + std::to_string(mean_H), cv::Point(50, 80), cv::FONT_HERSHEY_SIMPLEX, 2, CV_RGB(0, 255, 255), 3);
	cv::putText(draw_img, "Color value : " + std::to_string(mean_H), cv::Point(50, 80), cv::FONT_HERSHEY_SIMPLEX, 2, CV_RGB(0, 0, 0), 3);		//display color 

	// Display wafer detection
	drawContours(hsv_image, contours, static_cast<int>(maxContourIndex), CV_RGB(255, 0, 255), 3, LINE_8, hierarchy, 2);
	cv::putText(hsv_image, "Detected wafer", cv::Point(60, 335), cv::FONT_HERSHEY_SIMPLEX, 2, CV_RGB(255, 0, 255), 3);

	int stop = 0;
}