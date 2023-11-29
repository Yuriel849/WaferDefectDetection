#pragma once
#include "InspGeneric.h"

class InspectContamination : public InspGeneric
{
public:
	InspectContamination();
	~InspectContamination();

	int OnTestProcess(const Mat& src, const Mat& drawColor, std::vector<cv::Rect>& vRegions) override;
private:
};
#pragma once
