#pragma once
#include "InspGeneric.h"

class InspectCrack : public InspGeneric
{
public:
	InspectCrack();
	~InspectCrack();
	void draw_line(cv::Mat& img, cv::Point pt1, cv::Point pt2, cv::Scalar color, int thickness, std::string style, int gap);
	int OnTestProcess(const Mat& src, const Mat& drawColor, std::vector<cv::Rect>& vChips, int& flaw_num) override;

	
	
	string GetName() { return _InspectName; }







private:
	int crack_num = 0;
	

	vector<Rect> find_crack; // crack 오류가 든 사각형 위치, 크기를 저장할 Rect형 vector
	vector<vector<Point>> contours_crack;
	vector<Vec4i> hierarchy_crack;
};