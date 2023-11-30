#include "InspectCrack.h"



InspectCrack::InspectCrack()
{
}

InspectCrack::~InspectCrack()
{
}

int InspectCrack::OnTestProcess(const Mat& src, const Mat& drawColor, std::vector<cv::Rect>* vRegions, int& flaw_num)
{
	// src			 탐색할 이미지 IMREAD_GRAYSCALE
	// drawColor	 사각형 채울 이미지
	// vRegions      색 채울 이미지 IMREAD_ANYCOLOR

	////Identify the large and small squares in each chip across the wafer under inspection///////////
	vector<cv::Rect> vRois; //전체 이미지 백터 배열로 저장
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	Mat obj_Region;
	Point ptThrehold = Point(888, 150); //이진화된 배경 색보다 밝을 경우 차이를 주기 위해 배경 위치 값 포인터로 설정 
	double min_threshold = src.data[ptThrehold.y * src.cols + ptThrehold.x] + 5; //배경색보다 밝을 부분 설정하여
	cv::threshold(src, obj_Region, min_threshold, 255, ThresholdTypes::THRESH_BINARY);// obj_Region에 이진화 작업
	contours.clear(); //새롭게 범위값을 구하기 위해 백터 포인트 변수 contourss 생성
	hierarchy.clear(); //백터 hierarchy변수 생성
	cv::findContours(obj_Region, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //객체 범위값을 구하기 위해 객체 외곽선 검출
	for (size_t k = 0; k < contours.size(); k++) //contourss.size();의 크기만큼 for문을 돌린다.(새롭게 정의된 객체 사이즈(수) 
	{
		double area = contourArea(contours[k]);

		RotatedRect rrt = minAreaRect(contours[k]);//객체 외곽선을 넘지 않을 만큼 의 사이즈를 wRrt에 담아줌
		Rect rt = rrt.boundingRect(); // 객체의 위치에 상관없이 정방향 사각형 생성

		if (160 <= rt.width && rt.width <= 200)
		{
			vRois.push_back(rt);
			cv::rectangle(drawColor, rrt.boundingRect(), CV_RGB(0, 255, 0), 2); //
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////

	RNG rng(12345);
	Scalar color_crack(0, 255, 255);		// 255, 0, 0

	for (size_t i = 0; i < vRois.size(); i++)
	{
		Scalar color;
		Mat binCrack;
		Mat subImgCrack = src(vRois[i]).clone();

		cv::Point ptThreshold(885, 150);
		cv::drawMarker(drawColor, ptThreshold, CV_RGB(255, 255, 255), MarkerTypes::MARKER_CROSS);
		double thres_crack = src.at<uchar>(ptThreshold.y, ptThreshold.x) + 9;					//////////////////// 61
		cv::threshold(subImgCrack, binCrack, thres_crack, 255, ThresholdTypes::THRESH_BINARY_INV);	//INV -> 43보다 어두운 거 + 61 

		//morpology 노이즈 제거 부분
		int kernelSz = 2; //노이즈 크기
		int shape = MorphShapes::MORPH_RECT; //
		cv::Size sz = Size(2 * kernelSz + 1, 2 * kernelSz + 1); //정lSz + 1) 
		Mat SE = cv::getStructuringElement(shape, sz); // 
		Mat src_open; //노이즈가 제거된 상태.
		int type = MorphTypes::MORPH_OPEN; // 노이즈를 제거하는 기능 MORPH_OPEN을 type에 담겠다.
		cv::morphologyEx(binCrack, src_open, type, SE);// cv::morphologyEx(src_bin(입력), src_open(출력), type, SE);//morphologyEx 노이즈를 제거하겠다. 

		cv::findContours(src_open, contours_crack, hierarchy_crack, RETR_TREE, CHAIN_APPROX_SIMPLE);

		string msg;
		msg = to_string(crack_num + 1);

		if (contours_crack.empty())
		{
			// color = color_flawless;
		}

		else
		{
			for (size_t k = 0; k < contours_crack.size(); k++) //contourss.size();의 크기만큼 for문을 돌린다.(새롭게 정의된 객체 사이즈(수) 
			{
				double area = contourArea(contours_crack[k]);

				double area_min = 10 * 10;
				if (area_min <= area)
				{
					vector<vector<Point>> contours_crack_error_all;
					vector<Point> contours_crack_error;
					for (size_t p = 0; p < contours_crack[k].size(); p++)
					{
						Point pt;
						pt.x = contours_crack[k].at(p).x + vRois[i].x;
						pt.y = contours_crack[k].at(p).y + vRois[i].y;
						contours_crack_error.push_back(pt);
					}
					contours_crack_error_all.push_back(contours_crack_error);
					drawContours(drawColor, contours_crack_error_all, 0, color_crack, 3);

					color = color_crack;
					crack_num += 1;
					cv::putText(drawColor, msg, vRois[i].br(), FONT_HERSHEY_SIMPLEX, 1, color, 2, 8);		//0, 255, 255
					cv::putText(drawColor, crack, vRois[i].tl(), FONT_HERSHEY_SIMPLEX, 0.5, color, 1.5, 8);
					cv::rectangle(drawColor, vRois[i], color, CV_FILLED);
					vRegions->push_back(vRois[i]); //crack 오류 사각형의 데이터를 저장. 

					/*int wid = (*vRegions)[i].width; : contamination 오류를 거르기 위해 chip들의 가로 세로 길이 저장함
					int he = (*vRegions)[i].height;
					int checking = 0;*/

					/*cv::Point rtPts[4] = { vRegions[i].tl(), Point(vRegions[i].tl().x + vRegions[i].width, vRegions[i].tl().y),
					vRegions[i].br(), Point(vRegions[i].tl().x, vRegions[i].br().y) };
					for (size_t c = 0; c < 4; c++)
					{
						draw_line(drawColor, rtPts[c % 4], rtPts[(c + 1) % 4], color, 2, "", 3);
					}*/
					flaw_num += 1;      // 결함 개수 증가 
				}
			}
		}
	}

	return 0;
}

void InspectCrack::draw_line(cv::Mat& img, cv::Point pt1, cv::Point pt2, cv::Scalar color, int thickness, std::string style, int gap)
{
#if 1
	float dx = pt1.x - pt2.x;
	float dy = pt1.y - pt2.y;

	float dist = sqrt(dx * dx + dy * dy);

	std::vector<cv::Point> pts;
	for (int i = 0; i < dist; i += gap)
	{
		float r = (float)i / dist;
		int x = int((pt1.x * (1.0 - r) + pt2.x * r) + .5);
		int y = int((pt1.y * (1.0 - r) + pt2.y * r) + .5);
		cv::Point p = cv::Point(x, y);
		pts.push_back(p);
	}

	int pts_size = pts.size();

	if (style == "dotted")
	{
		for (int i = 0; i < pts_size; i++)
		{
			cv::circle(img, pts[i], thickness, color, -1);
		}
	}
	else
	{
		cv::Point s = pts[0];
		cv::Point e = pts[0];

		//int count = 0;

		for (int i = 0; i < pts_size; i++)
		{
			s = e;
			e = pts[i];

			if (i % 2 == 1)
			{
				cv::line(img, s, e, color, thickness);
			}
		}
	}
#endif
}