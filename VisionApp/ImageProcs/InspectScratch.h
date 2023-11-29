#pragma once
#include "InspGeneric.h"

class InspectScratch : public InspGeneric
{
public:
	InspectScratch();
	~InspectScratch();

	int OnTestProcess(const Mat& src, const Mat& drawColor, void* pUserData) override;
private:

};
