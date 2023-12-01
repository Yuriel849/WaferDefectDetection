#pragma once

#include "common.h"
#include "InspectScratch.h"
#include "InspectCrack.h"
#include "InspectContamination.h"

std::string FLAWLESS = "./res/img/final_real_wafer.png";
std::string DONUT = "./res/img/donut_black.png";
std::string EDGE_LOCATION = "./res/img/edge_location_black.png";
std::string LOCATION = "./res/img/location_black.png";
std::string SCRATCH = "./res/img/scratch_black.png";
std::string SCRATCH_GREEN = "./res/img/Scratch_green.bmp";
std::string LOCATION_GREEN = "./res/img/Location_green.bmp";
std::string EDGE_LOCATION_NAVY = "./res/img/Edge_location_navy.bmp";
std::string DONUT_NAVY = "./res/img/Donut_navy.bmp";

void Wafer_color(string& filePath);
void QR_Detect(cv::Mat& draw_img, cv::Mat& fill_img);

void main()
{
	InspectScratch insp_scratch;
	InspectCrack insp_crack;
	InspectContamination insp_cont;

	cv::Mat src = cv::imread(DONUT_NAVY, cv::ImreadModes::IMREAD_GRAYSCALE);
	cv::Mat drawing = cv::imread(DONUT_NAVY, cv::ImreadModes::IMREAD_ANYCOLOR);

	Wafer_color(DONUT_NAVY);		//색샹 검출 함수 호출

	QR_Detect(drawing, drawing);		//QR 검출 함수 호출

	//InspectScratch* pins = &insp_scratch;
	//InspGeneric* pins = &insp_scratch;
	//InspGeneric* ptr[3] = { &insp_scratch,&insp_contRe,&insp_cont };

	std::vector<InspGeneric*> vInsps;
	vInsps.push_back(&insp_scratch);
	vInsps.push_back(&insp_crack);
	vInsps.push_back(&insp_cont);

	std::vector<std::vector<cv::Rect>*> vVRegions;
	std::vector<cv::Rect> Scratch_Regions;
	std::vector<cv::Rect> Crack_Regions;
	std::vector<cv::Rect> Cont_Regions;
	vVRegions.push_back(&Scratch_Regions);
	vVRegions.push_back(&Crack_Regions);
	vVRegions.push_back(&Cont_Regions);

	for (size_t i = 0; i < vInsps.size(); i++)
	{
		InspGeneric* pins = vInsps[i];
		std::vector<cv::Rect>* vRegions = vVRegions[i];

		pins->OnTestProcess(src, drawing, vRegions);
	}


	// Display imag
	namedWindow("Wafer Defect Detection", WINDOW_NORMAL);
	cv::resizeWindow("Wafer Defect Detection", 1200, 800);
	imshow("Wafer Defect Detection", drawing);
	waitKey();
}

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

void QR_Detect(cv::Mat& draw_img, cv::Mat& fill_img)
{
	// draw_img		 QR 검출할 이미지 IMREAD_ANYCOLOR
	// fill_img	     QR 검출할 이미지 IMREAD_ANYCOLOR

	QRCodeDetector QRdetector;

	vector<Point> QR_points;
	String QR_info = QRdetector.detectAndDecode(draw_img, QR_points);

	if (!QR_info.empty())
	{
		// polylines(src_QR_test, points, true, Scalar(0, 0, 255), 2);
		// putText(src_QR_test, info, Point(10, 30), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 0, 255));
		polylines(draw_img, QR_points, true, Scalar(0, 0, 255), 3);
		putText(draw_img, QR_info, QR_points[0], FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255));
		polylines(fill_img, QR_points, true, Scalar(0, 0, 255), 3);
		putText(fill_img, QR_info, QR_points[0], FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255));
	}
}