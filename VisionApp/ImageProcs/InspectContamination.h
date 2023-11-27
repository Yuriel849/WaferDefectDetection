#pragma once
#include "InspGeneric.h"

class InspectContamination : public InspGeneric
{
public:
	InspectContamination();
	~InspectContamination();
	int OnTestProcess(const Mat& src, const Mat& drawColor, void* pUserData) override;
	int GetObjectRegionsTemplatMatching(const Mat& src, const Mat& ptrn, std::vector<cv::Rect>& vRegions);
private:
	const string TEMPLATE = "./res/img/wafer_template.png";
	bool defectFlag = false;
	Mat& mask(int boundary1, int boundary2, Mat& masked, Mat& target);
};
#pragma once
