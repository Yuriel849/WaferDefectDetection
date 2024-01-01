#include "InspectQR.h"



InspectQR::InspectQR()
{
	_InspectName = "QR";
}

InspectQR::~InspectQR()
{

}

int InspectQR::OnTestProcess(const Mat& src, const Mat& drawColor, std::vector<cv::Rect>& vChips, int& flaw_num)
{
	QRDetect(src);

	return 0;
}


void InspectQR::QRDetect(const cv::Mat& src)
{
	// draw_img       QR 검출할 이미지 IMREAD_ANYCOLOR
	// fill_img        QR 검출할 이미지 IMREAD_ANYCOLOR

	QRCodeDetector QRdetector;

	vector<Point> QRpoints;
	String QRinfo = QRdetector.detectAndDecode(src, QRpoints);

	strQRCode = QRinfo;
	int x = QRpoints[0].x;
	int y = QRpoints[0].y;
	int w = QRpoints[2].x - QRpoints[0].x + 1;
	int h = QRpoints[2].y - QRpoints[0].y + 1;
	rtQRArea = cv::Rect(x, y, w,h);
	if (!QRinfo.empty())
	{
		// polylines(src_QR_test, points, true, Scalar(0, 0, 255), 2);
		// putText(src_QR_test, info, Point(10, 30), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 0, 255));
		
		
		//polylines(drawImg, QRpoints, true, Scalar(0, 255, 0), 4);
		//putText(drawImg, QRinfo, QRpoints[0], FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 255, 0), 2);
		
		
		
		// polylines(fill_img, QRpoints, true, Scalar(0, 0, 255), 3);
		// putText(fill_img, QRinfo, QRpoints[0], FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 0, 255));


		SetQRArea(rtQRArea);
		SetQRCode(strQRCode);
	}
}