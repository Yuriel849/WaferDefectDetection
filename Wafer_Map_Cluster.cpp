
//불량칩을 표시한 웨이퍼맵을 영상처리 
//noise성 불량칩을 제거하고, 진성 불량칩의 군집(cluster)을 나타내기

#pragma once
#include "Common.h"

void main()
{
	std::string filePath = "../KCCImageNet/project_wafer/L_wafermap.bmp";	

	std::string filePaths[] = {
		"../KCCImageNet/project_wafer/L_wafermap.bmp",			//filePath_Location
		"../KCCImageNet/project_wafer/S_wafermap.bmp",			//filePath_Scratch
		"../KCCImageNet/project_wafer/E_wafermap.bmp",			//filePath_Edge_Location
		"../KCCImageNet/project_wafer/D_wafermap.bmp"			//filePath_Donut
	};

	int fileCount = sizeof(filePaths) / sizeof(filePaths[0]);
	for (size_t f = 0; f < fileCount; f++)
	{
		filePath = filePaths[f];
		cv::Mat src_gray;	
		Mat src_default = cv::imread(filePath, cv::ImreadModes::IMREAD_COLOR);
		cv::resize(src_default, src_default, Size(std::max(src_default.cols, src_default.rows), std::max(src_default.cols, src_default.rows)));		//이미지 사이즈를 500*500으로 resize(가로 세로중 더 큰 값으로)
		cvtColor(src_default, src_gray, CV_BGR2GRAY);

		Mat drawing = src_default.clone();

		uchar* pData = src_gray.data;
		size_t width = src_gray.cols;
		size_t height = src_gray.rows;

		//fail chip인 노란색만 남기기 위한 threshold
		int threshold_min = 210;
		int threshold_max = 255;

		cv::Mat src_bin = Mat::zeros(cv::Size(width, height), CV_8UC1);
		uchar* pDataBin = src_bin.data;

		//Binary
		cv::threshold(src_gray, src_bin, threshold_min, threshold_max, ThresholdTypes::THRESH_BINARY);


		//fail chip contours
		RNG rng(12345);
		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;
		findContours(src_bin, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		//check contours display
		for (size_t i = 0; i < contours.size(); i++)
		{
			Scalar color = Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
		}

		//debugging display
		double area_min = 1500;			//노이즈성 fail chip을 제거하기 위한 threshold
		std::vector<size_t> vMasks;		//진성 fail chip의 군집인 max area index 가 N개 나옴
		for (size_t i = 0; i < contours.size(); i++)
		{
			double area = contourArea(contours[i]);
			if (area < area_min) continue;

			//Identify squares with minimum area bounding rectangle
			RotatedRect rrt = minAreaRect(contours[i]);

			vMasks.push_back(i);
			Point ptTxt = Point(rrt.boundingRect().tl());
		}

		vector<Point> ErrRegion_contour;
		cv::Mat mask_region = Mat::zeros(src_bin.size(), CV_8UC1);
		for (size_t i = 0; i < vMasks.size(); i++)
		{
			size_t index = vMasks[i];	//max area index...N
			Scalar color = Scalar(255, 255, 255);	
			drawContours(mask_region, contours, (int)index, color, cv::FILLED);

			for (size_t k = 0; k < contours[index].size(); k++)
			{
				ErrRegion_contour.push_back(contours[index].at(k));
			}
		}

		if (ErrRegion_contour.size() == 0)
			continue;																			//break point
		vector<vector<Point>> ErrRegion_contours;
		ErrRegion_contours.push_back(ErrRegion_contour);
		drawContours(drawing, ErrRegion_contours, (int)0, CV_RGB(0, 0, 255), cv::FILLED);
		RotatedRect rrt = minAreaRect(ErrRegion_contour);
		Point region_center = minAreaRect(ErrRegion_contour).center;
		drawMarker(drawing, region_center, CV_RGB(255, 0, 255), MarkerTypes::MARKER_CROSS, std::max(rrt.size.width, rrt.size.height));
		const int ptSz = 4;
		Point2f pt[ptSz];
		rrt.points(pt);
		for (size_t i = 0; i < ptSz; i++)
		{
			cv::line(drawing, pt[i % ptSz], pt[(i + 1) % ptSz], CV_RGB(255, 0, 255), 1);
		}

		cv::ellipse(drawing, rrt, CV_RGB(255, 0, 255), 1);
		double major_axis_length = std::max(rrt.size.width, rrt.size.height) / 2;
		double minor_axis_length = std::min(rrt.size.width, rrt.size.height) / 2;
		double ratio = minor_axis_length / major_axis_length;						//비율:단축/장축 -> 불량칩 군집의 형태 파악


		Point ptTxt = Point(10, 20);	//이미지 좌상에 디스플레이(면적 / 무게중심x,y / 비율(단축/장축) / 단축과 장축 길이 / 각도)
		string msg;
		double area = contourArea(ErrRegion_contour);
		msg = std::format("1. area = {:.1f}", area);
		putText(drawing, msg, Point(ptTxt.x, ptTxt.y + 20 * 0), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(255, 255, 255), 1, 8);
		msg = std::format("2. x,y = {:d} {:d}", region_center.x, region_center.y);
		putText(drawing, msg, Point(ptTxt.x, ptTxt.y + 20 * 1), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(255, 255, 255), 1, 8);
		msg = std::format("3. ratio = {:0.3f}", ratio);
		putText(drawing, msg, Point(ptTxt.x, ptTxt.y + 20 * 2), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(255, 255, 255), 1, 8);
		msg = std::format("4. L1, L2 = {:0.3f} {:0.3f}", minor_axis_length, major_axis_length);
		putText(drawing, msg, Point(ptTxt.x, ptTxt.y + 20 * 3), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(255, 255, 255), 1, 8);
		msg = std::format("5. theta = {:0.3f}", rrt.angle);
		putText(drawing, msg, Point(ptTxt.x, ptTxt.y + 20 * 4), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(255, 255, 255), 1, 8);


		int fs = 0;																				//break point

		//편집된 drawing 이미지를 파일로 저장
		size_t pos = filePath.find_last_of('.');
		string resultfilePaths = filePath.substr(0, pos) + "_result.bmp";	//저장할 이미지 파일 경로
		cv::imwrite(resultfilePaths, drawing);

	}

	int a = 0;
}
