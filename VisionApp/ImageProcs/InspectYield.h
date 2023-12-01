#pragma once
#include "InspGeneric.h"

class InspectYield
{
public:
	InspectYield();
	~InspectYield();
	double allChips = 40;
	int yield = 0;

	void CalculateYield(int& flaw_num, cv::Mat& draw_img);
private:
};