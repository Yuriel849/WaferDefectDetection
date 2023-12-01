#pragma once
#include "InspGeneric.h"

class InspectCrack : public InspGeneric
{
public:
	InspectCrack();
	~InspectCrack();
	void draw_line(cv::Mat& img, cv::Point pt1, cv::Point pt2, cv::Scalar color, int thickness, std::string style, int gap);
	int OnTestProcess(const Mat& src, const Mat& drawColor, std::vector<cv::Rect>* vRegions, int& flaw_num) override;
	int crack_num = 0;
	string crack = "Crack";

	vector<Rect> find_crack; // crack ������ �� �簢�� ��ġ, ũ�⸦ ������ Rect�� vector

private:
	vector<vector<Point>> contours_crack;
	vector<Vec4i> hierarchy_crack;
};