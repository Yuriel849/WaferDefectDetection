#include "InspGeneric.h"

InspGeneric::InspGeneric()
{
}

InspGeneric::~InspGeneric()
{
}

int InspGeneric::OnTestProcess(const Mat& src, const Mat& drawColor, void* pUserData)
{
    return 0;
}

int InspGeneric::GetObjectRegionsTemplatMatching(const Mat& src, const Mat& ptrn, std::vector<cv::Rect>& vRegions)
{
    return 0;
}
