#pragma once

#include "Common.h"
class InspGeneric
{
public:
	InspGeneric();
	virtual ~InspGeneric();

	virtual int OnTestProcess(const Mat& src, const Mat& drawColor, void* pUserData)=0;
protected:
private:
	const string PATTERN = "./res/img/wafer_template.png";
};

