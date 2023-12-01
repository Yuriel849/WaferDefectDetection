#include "InspectQR.h"



InspectQR::InspectQR()
{

}

InspectQR::~InspectQR()
{

}


void InspectQR::QRDetect(cv::Mat& draw_img)
{
	// draw_img       QR 검출할 이미지 IMREAD_ANYCOLOR
	// fill_img        QR 검출할 이미지 IMREAD_ANYCOLOR

	QRCodeDetector QRdetector;

	vector<Point> QRpoints;
	String QRinfo = QRdetector.detectAndDecode(draw_img, QRpoints);

	if (!QRinfo.empty())
	{
		// polylines(src_QR_test, points, true, Scalar(0, 0, 255), 2);
		// putText(src_QR_test, info, Point(10, 30), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 0, 255));
		polylines(draw_img, QRpoints, true, Scalar(0, 255, 0), 4);
		putText(draw_img, QRinfo, QRpoints[0], FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 255, 0), 2);
		// polylines(fill_img, QRpoints, true, Scalar(0, 0, 255), 3);
		// putText(fill_img, QRinfo, QRpoints[0], FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 0, 255));
	}
}