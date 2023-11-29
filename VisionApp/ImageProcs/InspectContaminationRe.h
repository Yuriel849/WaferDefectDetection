#pragma once
#include "InspGeneric.h"

class InspectContaminationRe : public InspGeneric
{
public:
	InspectContaminationRe();
	~InspectContaminationRe();
	int OnTestProcess(const Mat& src, const Mat& drawColor, std::vector<cv::Rect> vRegions) override;
	int GetObjectRegionsTemplatMatching(const Mat& src, const Mat& ptrn, std::vector<cv::Rect>& vRegions);
private:
	const string PATTERN = "./res/img/wafer_template.png";
};
#pragma once
