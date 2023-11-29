#pragma once
#include "InspGeneric.h"

class InspectCrack : public InspGeneric
{
public:
	InspectCrack();
	~InspectCrack();

	int OnTestProcess(const Mat& src, const Mat& drawColor, std::vector<cv::Rect> vRegions) override;
private:

};
