#pragma once
#include "InspGeneric.h"

class InspectScratch : public InspGeneric
{
public:
	InspectScratch();
	~InspectScratch();
	void draw_line(cv::Mat& img, cv::Point pt1, cv::Point pt2, cv::Scalar color, int thickness, std::string style, int gap);
	int OnTestProcess(const Mat& src, const Mat& drawColor, std::vector<cv::Rect>* vRegions, int& flaw_num) override;
	int scratch_num = 0;
	string scratch = "Scratch";

	vector<Rect> find_scratch; // crack 오류가 든 사각형 위치, 크기를 저장할 Rect형 vector
private:

	vector<vector<Point>> contours_scratch;
	vector<Vec4i> hierarchy_scratch;
};