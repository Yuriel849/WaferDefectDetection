#pragma once

#include "Common.h"
class InspGeneric
{
public:
	InspGeneric();
	virtual ~InspGeneric();

	virtual int OnTestProcess(const Mat& src, const Mat& drawColor, std::vector<cv::Rect>* vRegions, int& flaw_num) = 0;
protected:
	const string PATTERN = "./res/img/wafer_template.png";
private:
};

