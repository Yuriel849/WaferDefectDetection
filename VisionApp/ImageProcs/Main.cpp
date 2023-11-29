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

void main()
{
	InspectScratch insp_scratch;
	InspectCrack insp_crack;
	InspectContamination insp_cont;

	cv::Mat src = cv::imread(SCRATCH_GREEN, cv::ImreadModes::IMREAD_GRAYSCALE);
	cv::Mat drawing = cv::imread(SCRATCH_GREEN, cv::ImreadModes::IMREAD_ANYCOLOR);

	//InspectScratch* pins = &insp_scratch;
	//InspGeneric* pins = &insp_scratch;
	//InspGeneric* ptr[3] = { &insp_scratch,&insp_contRe,&insp_cont };

	std::vector<InspGeneric*> vInsps;
	vInsps.push_back(&insp_scratch);
	vInsps.push_back(&insp_crack);
	vInsps.push_back(&insp_cont);

	std::vector<std::vector<cv::Rect>> vVRegions;

	for (size_t i = 0; i < vInsps.size(); i++)
	{
		InspGeneric* pins = vInsps[i];
		std::vector<cv::Rect> vRegions = vVRegions[i];

		pins->OnTestProcess(src, drawing, vRegions);

	}

	// Display imag
	namedWindow("Wafer Defect Detection", WINDOW_NORMAL);
	cv::resizeWindow("Wafer Defect Detection", 1200, 800);
	imshow("Wafer Defect Detection", drawing);
	waitKey();

}
