#pragma once
#include "Common.h"


std::string Templt = "../KCCImageNet/WaferProj/wafer_chip_template.png";
std::string flawless = "../KCCImageNet/WaferProj/final_real_wafer.png";
std::string Donut_Black = "../KCCImageNet/WaferProj/donut_black.png";
std::string Scratch_Black = "../KCCImageNet/WaferProj/scratch_black.png";
std::string Location_Black = "../KCCImageNet/WaferProj/location_black.png";
std::string Edge_Location_Black = "../KCCImageNet/WaferProj/edge_location_black.png";

std::string Scratch_Green = "../KCCImageNet/WaferProj/Scratch_green.bmp";
std::string Location_Green = "../KCCImageNet/WaferProj/Location_green.bmp";
std::string Donut_Navy = "../KCCImageNet/WaferProj/Donut_navy.bmp";
std::string Edge_Location_Navy = "../KCCImageNet/WaferProj/Edge_location_navy.bmp";

std::string TEST = "../KCCImageNet/WaferProj/Donut_navy.bmp";		// 다른 파일을 빨리 확인하기 위해서 선언, 파일명만 바꾸면 바로 동작시킬 수 있도록 


void draw_line(cv::Mat& img, cv::Point pt1, cv::Point pt2, cv::Scalar color, int thickness, std::string style, int gap)
{
#if 1
	float dx = pt1.x - pt2.x;
	float dy = pt1.y - pt2.y;

	float dist = sqrt(dx * dx + dy * dy);

	std::vector<cv::Point> pts;
	for (int i = 0; i < dist; i += gap)
	{
		float r = (float)i / dist;
		int x = int((pt1.x * (1.0 - r) + pt2.x * r) + .5);
		int y = int((pt1.y * (1.0 - r) + pt2.y * r) + .5);
		cv::Point p = cv::Point(x, y);
		pts.push_back(p);
	}

	int pts_size = pts.size();

	if (style == "dotted")
	{
		for (int i = 0; i < pts_size; i++)
		{
			cv::circle(img, pts[i], thickness, color, -1);
		}
	}
	else
	{
		cv::Point s = pts[0];
		cv::Point e = pts[0];

		//int count = 0;

		for (int i = 0; i < pts_size; i++)
		{
			s = e;
			e = pts[i];

			if (i % 2 == 1)
			{
				cv::line(img, s, e, color, thickness);
			}
		}
	}
#endif
}


void ScratchDetect(vector<Rect>& find, cv::Mat& search_img, cv::Mat& draw_img, cv::Mat& fill_img)
{
	// find			 좌표 배열 
	// search_img	 탐색할 이미지 IMREAD_GRAYSCALE
	// draw_img		 사각형 그릴 이미지 IMREAD_ANYCOLOR
	// fill_img	     색 채울 이미지 IMREAD_ANYCOLOR

	RNG rng(12345);
	vector<vector<Point>> contours_scratch;
	vector<Vec4i> hierarchy_scratch;
	//Scalar color_flawless(255, 255, 0);
	Scalar color_scratch(0, 0, 255);

	int scratch_num = 1;
	string scratch = "Scratch";

	for (size_t i = 0; i < find.size(); i++)
	{
		Scalar color;
		Mat binScratch;
		Mat subImgScratch = search_img(find[i]).clone();

		//double thres_scratch = 210;
		cv::Point ptThreshold(817, 250);
		double thres_scratch = search_img.at<uchar>(ptThreshold.y, ptThreshold.x) + 60;
		cv::threshold(subImgScratch, binScratch, thres_scratch, 255, ThresholdTypes::THRESH_BINARY);
		cv::findContours(binScratch, contours_scratch, hierarchy_scratch, RETR_TREE, CHAIN_APPROX_SIMPLE);

		string msg;
		msg = to_string(scratch_num);

		if (contours_scratch.empty())
		{
			// color = color_flawless;
		}

		else
		{
			for (size_t k = 0; k < contours_scratch.size(); k++) // contourss.size();의 크기만큼 for문을 돌린다.(새롭게 정의된 객체 사이즈(수) 
			{
				double area = contourArea(contours_scratch[k]);

				double area_min = 10 * 10;
				if (area_min <= area)
				{
					vector<vector<Point>> contours_scratch_error_all;
					vector<Point> contours_scratch_error;
					for (size_t p = 0; p < contours_scratch[k].size(); p++)
					{
						Point pt;
						pt.x = contours_scratch[k].at(p).x + find[i].x;
						pt.y = contours_scratch[k].at(p).y + find[i].y;
						contours_scratch_error.push_back(pt);
					}
					contours_scratch_error_all.push_back(contours_scratch_error);
					drawContours(draw_img, contours_scratch_error_all, 0, color_scratch, 5);



					color = color_scratch;
					scratch_num += 1;
					cv::putText(draw_img, msg, find[i].br(), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(255, 0, 0), 1.5, 8);		//0, 255, 255
					cv::putText(draw_img, scratch, find[i].tl(), FONT_HERSHEY_SIMPLEX, 0.5, color, 1.5, 8);
					cv::rectangle(fill_img, find[i], color, CV_FILLED);

					cv::Point rtPts[4] = { find[i].tl(), Point(find[i].tl().x + find[i].width, find[i].tl().y),
					find[i].br(), Point(find[i].tl().x, find[i].br().y) };
					for (size_t c = 0; c < 4; c++)
					{
						draw_line(draw_img, rtPts[c % 4], rtPts[(c + 1) % 4], color, 2, "", 3);
					}
				}
			}
		}
	}
}


void CrackDetect(vector<Rect>& find, cv::Mat& search_img, cv::Mat& draw_img, cv::Mat& fill_img)
{
	// find			좌표 배열 
	// search_img	 탐색할 이미지 IMREAD_GRAYSCALE
	// draw_img		 사각형 그릴 이미지 IMREAD_ANYCOLOR
	// fill_img	     색 채울 이미지 IMREAD_ANYCOLOR

	RNG rng(12345);
	vector<vector<Point>> contours_crack;
	vector<Vec4i> hierarchy_crack;
	// Scalar color_flawless(0, 255, 0);
	Scalar color_crack(0, 255, 255);		// 255, 0, 0

	int crack_num = 1;
	string crack = "Crack";

	for (size_t i = 0; i < find.size(); i++)
	{
		Scalar color;
		Mat binCrack;
		Mat subImgCrack = search_img(find[i]).clone();

		cv::Point ptThreshold(885, 150);
		cv::drawMarker(draw_img, ptThreshold, CV_RGB(255, 255, 255), MarkerTypes::MARKER_CROSS);
		double thres_crack = search_img.at<uchar>(ptThreshold.y, ptThreshold.x) + 9;
		cv::threshold(subImgCrack, binCrack, thres_crack, 255, ThresholdTypes::THRESH_BINARY_INV);

		// morpology 노이즈 제거 부분
		int kernelSz = 2;		// 노이즈 크기
		int shape = MorphShapes::MORPH_RECT;
		cv::Size sz = Size(2 * kernelSz + 1, 2 * kernelSz + 1); // 정lSz + 1) 
		Mat SE = cv::getStructuringElement(shape, sz);
		Mat src_open;		 // 노이즈가 제거된 상태.
		int type = MorphTypes::MORPH_OPEN;		// 노이즈를 제거하는 기능 MORPH_OPEN을 type에 담겠다.
		cv::morphologyEx(binCrack, src_open, type, SE);// cv::morphologyEx(src_bin(입력), src_open(출력), type, SE);//morphologyEx 노이즈를 제거하겠다. 

		cv::findContours(src_open, contours_crack, hierarchy_crack, RETR_TREE, CHAIN_APPROX_SIMPLE);

		string msg;
		msg = to_string(crack_num);

		if (contours_crack.empty())
		{
			// color = color_flawless;
		}

		else
		{
			for (size_t k = 0; k < contours_crack.size(); k++)		// contourss.size();의 크기만큼 for문을 돌린다.(새롭게 정의된 객체 사이즈(수) 
			{
				double area = contourArea(contours_crack[k]);

				double area_min = 10 * 10;
				if (area_min <= area)
				{
					vector<vector<Point>> contours_crack_error_all;
					vector<Point> contours_crack_error;
					for (size_t p = 0; p < contours_crack[k].size(); p++)
					{
						Point pt;
						pt.x = contours_crack[k].at(p).x + find[i].x;
						pt.y = contours_crack[k].at(p).y + find[i].y;
						contours_crack_error.push_back(pt);
					}
					contours_crack_error_all.push_back(contours_crack_error);
					drawContours(draw_img, contours_crack_error_all, 0, color_crack, 3);


					color = color_crack;
					crack_num += 1;
					cv::putText(draw_img, msg, find[i].br(), FONT_HERSHEY_SIMPLEX, 0.5, color, 1.5, 8);		//0, 255, 255
					cv::putText(draw_img, crack, find[i].tl(), FONT_HERSHEY_SIMPLEX, 0.5, color, 1.5, 8);
					cv::rectangle(fill_img, find[i], color, CV_FILLED);

					cv::Point rtPts[4] = { find[i].tl(), Point(find[i].tl().x + find[i].width, find[i].tl().y),
					find[i].br(), Point(find[i].tl().x, find[i].br().y) };
					for (size_t c = 0; c < 4; c++)
					{
						draw_line(draw_img, rtPts[c % 4], rtPts[(c + 1) % 4], color, 2, "", 3);
					}
				}
			}
		}
	}
}


void QR_Detect(cv::Mat& draw_img, cv::Mat& fill_img)
{
	// draw_img		 QR 검출할 이미지 IMREAD_ANYCOLOR
	// fill_img	     QR 검출할 이미지 IMREAD_ANYCOLOR

	QRCodeDetector QRdetector;   

	vector<Point> QRpoints;      
	String QRinfo = QRdetector.detectAndDecode(draw_img, QRpoints);		

	if (!QRinfo.empty())
	{
		// polylines(src_QR_test, points, true, Scalar(0, 0, 255), 2);
		// putText(src_QR_test, info, Point(10, 30), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 0, 255));
		polylines(draw_img, QRpoints, true, Scalar(255, 0, 255), 3);
		putText(draw_img, QRinfo, QRpoints[0], FONT_HERSHEY_SIMPLEX, 0.8, Scalar(255, 0, 255));
		polylines(fill_img, QRpoints, true, Scalar(255, 0, 255), 3);
		putText(fill_img, QRinfo, QRpoints[0], FONT_HERSHEY_SIMPLEX, 0.8, Scalar(255, 0, 255));
	}
}


void main()
{
	cv::Mat src_gray_search = cv::imread(TEST, cv::ImreadModes::IMREAD_GRAYSCALE);
	cv::Mat src_draw = cv::imread(TEST, cv::ImreadModes::IMREAD_ANYCOLOR);
	cv::Mat src_filled = src_draw.clone();

	QR_Detect(src_draw, src_filled);

	{
		// find objects

		vector<cv::Rect> vRois;
		Mat obj_Region;
		Point ptThrehold = Point(888, 150);
		double min_threshold = src_gray_search.data[ptThrehold.y * src_gray_search.cols + ptThrehold.x] + 5;
		cv::threshold(src_gray_search, obj_Region, min_threshold, 255, ThresholdTypes::THRESH_BINARY);
		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;
		cv::findContours(obj_Region, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);		

		for (size_t k = 0; k < contours.size(); k++)		
		{
			double area = contourArea(contours[k]);

			RotatedRect rrt = minAreaRect(contours[k]);		
			Rect rt = rrt.boundingRect();		

			double area_min = 180 * 180 - 180 * 180 * 0.1;
			double area_max = 180 * 180 + 180 * 180 * 0.1;
			if (area_min <= area && area <= area_max)
			{

				vRois.push_back(rt);
				cv::rectangle(src_draw, rrt.boundingRect(), CV_RGB(0, 0, 255), 1);		
			}
		}

		ScratchDetect(vRois, src_gray_search, src_draw, src_filled);
		CrackDetect(vRois, src_gray_search, src_draw, src_filled);
	}
}
