#pragma once
#include "InspGeneric.h"

class InspectContamination : public InspGeneric
{
public:
	InspectContamination();
	~InspectContamination();

	int OnTestProcess(const Mat& src, const Mat& drawColor, void* pUserData) override;
private:
};
#pragma once
