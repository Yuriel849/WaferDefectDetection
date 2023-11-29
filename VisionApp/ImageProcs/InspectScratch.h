#pragma once
#include "InspGeneric.h"

class InspectScratch : public InspGeneric
{
public:
	InspectScratch();
	~InspectScratch();

	int OnTestProcess(const Mat& src, const Mat& drawColor, std::vector<cv::Rect>& vRegions) override;
private:

};
