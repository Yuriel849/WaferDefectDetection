#pragma once

#include "Common.h"
class InspGeneric
{
public:
	InspGeneric();
	~InspGeneric();

	virtual int OnTestProcess(const Mat& src, const Mat& drawColor, void* pUserData);
	virtual int GetObjectRegionsTemplatMatching(const Mat& src, const Mat& ptrn, std::vector<cv::Rect>& vRegions);
protected:
private:

};

