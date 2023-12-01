#include "InspectYield.h"


InspectYield::InspectYield()
{

}

InspectYield::~InspectYield()
{

}


void InspectYield::CalculateYield(int& flaw_num, cv::Mat& draw_img)
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