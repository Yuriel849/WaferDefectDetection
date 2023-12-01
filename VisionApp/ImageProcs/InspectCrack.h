#pragma once
#include "InspGeneric.h"

class InspectCrack : public InspGeneric
{
public:
	InspectCrack();
	~InspectCrack();
	void draw_line(cv::Mat& img, cv::Point pt1, cv::Point pt2, cv::Scalar color, int thickness, std::string style, int gap);
	int OnTestProcess(const Mat& src, const Mat& drawColor, std::vector<cv::Rect>* vRegions) override;
	int crack_num = 1;
	string crack = "Crack";

	vector<Rect> find_crack; // crack ?§Î•òÍ∞Ä ???¨Í∞Å???ÑÏπò, ?¨Í∏∞Î•??Ä?•Ìï† Rect??vector
private:
	vector<vector<Point>> contours_crack;
	vector<Vec4i> hierarchy_crack;
};
