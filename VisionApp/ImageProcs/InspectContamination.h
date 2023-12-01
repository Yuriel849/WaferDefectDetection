#pragma once
#include "InspGeneric.h"

class InspectContamination : public InspGeneric
{
public:
	InspectContamination();
	~InspectContamination();
	int OnTestProcess(const Mat& src, const Mat& drawColor, std::vector<cv::Rect>* vRegions, int& flaw_num) override;
	int con_num = 0;
	string contamination = "Contamination";
private:
};
#pragma once