#pragma once

#include "common.h"
#include "InspectScratch.h"
#include "InspectCrack.h"
#include "InspectContamination.h"
#include "InspectQR.h"
#include "InspectYield.h"

std::string FLAWLESS = "./res/img/final_real_wafer.png";
std::string DONUT = "./res/img/donut_black.png";
std::string EDGE_LOCATION = "./res/img/edge_location_black.png";
std::string LOCATION = "./res/img/location_black.png";
std::string SCRATCH = "./res/img/scratch_black.png";

std::string SCRATCH_GREEN = "./res/img/Scratch_green.bmp";
std::string LOCATION_GREEN = "./res/img/Location_green.bmp";
std::string EDGE_LOCATION_NAVY = "./res/img/Edge_location_navy.bmp";
std::string DONUT_NAVY = "./res/img/Donut_navy.bmp";



void main()
{
	InspectScratch insp_scratch;
	InspectCrack insp_crack;
	InspectContamination insp_cont;
	InspectQR insp_QR;
	InspectYield insp_Y;

	int flaw_num = 0;

	cv::Mat src = cv::imread(DONUT_NAVY, cv::ImreadModes::IMREAD_GRAYSCALE);
	cv::Mat drawing = cv::imread(DONUT_NAVY, cv::ImreadModes::IMREAD_ANYCOLOR);

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

	insp_QR.QRDetect(drawing);

	for (size_t i = 0; i < vInsps.size(); i++)
	{
		InspGeneric* pins = vInsps[i];
		std::vector<cv::Rect>* vRegions = vVRegions[i];

		pins->OnTestProcess(src, drawing, vRegions, flaw_num);
	}
	// cout << flaw_num << endl;	// 이 칩의 총 결함 개수가 제대로 저장되었는가 확인 
	insp_Y.CalculateYield(flaw_num, drawing);

	// Display imag
	namedWindow("Wafer Defect Detection", WINDOW_NORMAL);
	cv::resizeWindow("Wafer Defect Detection", 1200, 800);
	imshow("Wafer Defect Detection", drawing);
	waitKey();

}