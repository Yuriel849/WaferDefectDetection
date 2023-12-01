#pragma once
#include "InspGeneric.h"

class InspectQR
{
public:
	InspectQR();
	~InspectQR();
	void QRDetect(cv::Mat& draw_img);

private:
};