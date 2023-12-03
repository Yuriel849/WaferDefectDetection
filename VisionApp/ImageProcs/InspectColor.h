#pragma once
#include "InspGeneric.h"

class InspectColor
{
public:
	InspectColor();
	~InspectColor();

	void DetectColor(cv::Mat& draw_img);
private:
};