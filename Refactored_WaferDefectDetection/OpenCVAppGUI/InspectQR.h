#pragma once
#include "InspGeneric.h"

class InspectQR : public InspGeneric
{
public:
	InspectQR();
	~InspectQR();
	int OnTestProcess(const Mat& src, const Mat& drawColor, std::vector<cv::Rect>& vChips, int& flaw_num);

	void QRDetect(const cv::Mat& src);

	string GetQRCode() {return strQRCode;}
	cv::Rect GetQRArea() {return rtQRArea;}
private:
	string strQRCode = "";
	cv::Rect rtQRArea;
};