//QR�ν�/������� ����� �Լ��� ����

#include "Common.h"

std::string filePath_Donut = "../KCCImageNet/project_wafer/Donut_navy.bmp";					//QR : WAFER_Y002
std::string filePath_Edge_Location = "../KCCImageNet/project_wafer/Edge_location_navy.bmp";	//QR : WAFER_Y001
std::string filePath_Location = "../KCCImageNet/project_wafer/Location_green.bmp";			//QR : WAFER_X001
std::string filePath_Scratch = "../KCCImageNet/project_wafer/Scratch_green.bmp";			//QR : WAFER_X002

void Wafer_color(string& filePath)		//wafer ���� ������ �̹���
{
	cv::Mat rgb_image = cv::imread(filePath, cv::IMREAD_COLOR);

	cv::Mat hsv_image;
	cv::cvtColor(rgb_image, hsv_image, cv::COLOR_BGR2HSV);		// RGB -> HSV ��ȯ

	cv::Scalar mean_hsv = cv::mean(hsv_image);

	double average_hue = mean_hsv[0];
	std::cout << "hue ��: " << average_hue << std::endl;

	Point ptTxt = Point(50, 80);
	string msg = std::format("Hue = {:.1f}", average_hue);
	putText(hsv_image, msg, Point(ptTxt.x, ptTxt.y), FONT_HERSHEY_SIMPLEX, 2, CV_RGB(0, 255, 255), 4, 8);

	int stop = 0;
}


void QR_Detect(cv::Mat& draw_img, cv::Mat& fill_img)
{
	// draw_img		 QR ������ �̹��� IMREAD_ANYCOLOR
	// fill_img	     QR ������ �̹��� IMREAD_ANYCOLOR

	QRCodeDetector QRdetector;

	vector<Point> QR_points;
	String QR_info = QRdetector.detectAndDecode(draw_img, QR_points);

	if (!QR_info.empty())
	{
		// polylines(src_QR_test, points, true, Scalar(0, 0, 255), 2);
		// putText(src_QR_test, info, Point(10, 30), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 0, 255));
		polylines(draw_img, QR_points, true, Scalar(0, 0, 255), 3);
		putText(draw_img, QR_info, QR_points[0], FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255));
		polylines(fill_img, QR_points, true, Scalar(0, 0, 255), 3);
		putText(fill_img, QR_info, QR_points[0], FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255));
	}
}

int main()
{
	cv::Mat draw = cv::imread(filePath_Donut, cv::IMREAD_ANYCOLOR);
	cv::Mat fill = cv::imread(filePath_Donut, cv::IMREAD_ANYCOLOR);

	Wafer_color(filePath_Donut);		//���� ���� �Լ� ȣ��

	QR_Detect(draw, fill);		//QR ���� �Լ� ȣ��

	int a = 0;
}

