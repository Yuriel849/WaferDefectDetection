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

	vector<Rect> find_scratch; // crack ������ �� �簢�� ��ġ, ũ�⸦ ������ Rect�� vector
private:

	vector<vector<Point>> contours_scratch;
	vector<Vec4i> hierarchy_scratch;
};