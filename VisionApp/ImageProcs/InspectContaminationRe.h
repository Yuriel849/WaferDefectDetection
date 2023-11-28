#pragma once
#include "InspGeneric.h"

class InspectContaminationRe : public InspGeneric
{
public:
	InspectContaminationRe();
	~InspectContaminationRe();
	int OnTestProcess(const Mat& src, const Mat& drawColor, void* pUserData) override;
	int GetObjectRegionsTemplatMatching(const Mat& src, const Mat& ptrn, std::vector<cv::Rect>& vRegions);
private:
	const string PATTERN = "./res/img/wafer_template.png";
	const string filePath_Search = "./res/img/Donut_navy.png"; //원본이미지
};
#pragma once
