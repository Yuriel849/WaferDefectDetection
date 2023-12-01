#pragma once
#include "InspGeneric.h"

class InspectScratch : public InspGeneric
{
public:
	InspectScratch();
	~InspectScratch();
	void draw_line(cv::Mat& img, cv::Point pt1, cv::Point pt2, cv::Scalar color, int thickness, std::string style, int gap);
	int OnTestProcess(const Mat& src, const Mat& drawColor, std::vector<cv::Rect>* vRegions) override;
	int scratch_num = 1;
	string scratch = "Scratch";

	vector<Rect> find_scratch; // crack ?§Î•òÍ∞Ä ???¨Í∞Å???ÑÏπò, ?¨Í∏∞Î•??Ä?•Ìï† Rect??vector
private:
	vector<vector<Point>> contours_scratch;
	vector<Vec4i> hierarchy_scratch;
};
