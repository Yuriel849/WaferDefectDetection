#pragma once
#include "InspGeneric.h"

class InspectYield : public InspGeneric
{
public:
	InspectYield();
	~InspectYield();

	int OnTestProcess(const Mat& src, const Mat& drawColor, std::vector<cv::Rect>& vChips, int& flaw_num);

	void CalculateYield(const cv::Mat& draw_img, int& flaw_num);

private:
	double allChips = 40;
	int yield = 0;
};