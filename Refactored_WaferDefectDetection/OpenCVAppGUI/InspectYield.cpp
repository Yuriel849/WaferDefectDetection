#include "InspectYield.h"


InspectYield::InspectYield()
{
	
	_InspectName = "Yield";
}

InspectYield::~InspectYield()
{

}

int InspectYield::OnTestProcess(const Mat& src, const Mat& drawColor, std::vector<cv::Rect>& vChips, int& flaw_num)
{
	CalculateYield(src, flaw_num);
	return 0;
}


void InspectYield::CalculateYield(const cv::Mat& draw_img, int& flaw_num)
{
	string msg1 = "Yield = ";
	string msg2;
	string msg3 = "%";
	yield = ((allChips - flaw_num) / allChips) * 100;
	cout << allChips - flaw_num << endl;
	cout << (allChips - flaw_num) / allChips << endl;
	msg2 = to_string(yield);

	putText(draw_img, msg1 + msg2 + msg3, cv::Point(50, 1625), FONT_HERSHEY_SIMPLEX, 2.5, Scalar(0, 0, 0), 3);
}