#include "InspectCrack.h"



InspectCrack::InspectCrack()
{
}

InspectCrack::~InspectCrack()
{
}

int InspectCrack::OnTestProcess(const Mat& src, const Mat& drawColor, std::vector<cv::Rect>* vRegions)
{
	// find			 MechingMethod()�?결함 ?�는 ?��?지�??�플�??�색?�여 ?��?, ?�바�?좌표 배열 
	// search_img	 ?�색???��?지 IMREAD_GRAYSCALE
	// draw_img		 ?�각??그릴 ?��?지 IMREAD_ANYCOLOR
	// fill_img	     ??채울 ?��?지   IMREAD_ANYCOLOR

	  ////Identify the large and small squares in each chip across the wafer under inspection///////////
	vector<cv::Rect> vRois; //?�체 ?��?지 백터 배열�??�??
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	Mat obj_Region;
	Point ptThrehold = Point(888, 150); //?�진?�된 배경 ?�보??밝을 경우 차이�?주기 ?�해 배경 ?�치 �??�인?�로 ?�정 
	double min_threshold = src.data[ptThrehold.y * src.cols + ptThrehold.x] + 5; //배경?�보??밝을 부�??�정?�여
	cv::threshold(src, obj_Region, min_threshold, 255, ThresholdTypes::THRESH_BINARY);// obj_Region???�진???�업
	contours.clear(); //?�롭�?범위값을 구하�??�해 백터 ?�인??변??contourss ?�성
	hierarchy.clear(); //백터 hierarchy변???�성
	cv::findContours(obj_Region, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //객체 범위값을 구하�??�해 객체 ?�곽??검�?
	for (size_t k = 0; k < contours.size(); k++) //contourss.size();???�기만큼 for문을 ?�린??(?�롭�??�의??객체 ?�이�??? 
	{
		double area = contourArea(contours[k]);

		RotatedRect rrt = minAreaRect(contours[k]);//객체 ?�곽?�을 ?��? ?�을 만큼 ???�이즈�? wRrt???�아�?
		Rect rt = rrt.boundingRect(); // 객체???�치???��??�이 ?�방???�각???�성

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
		cv::threshold(subImgCrack, binCrack, thres_crack, 255, ThresholdTypes::THRESH_BINARY_INV);	//INV -> 43보다 ?�두??�?+ 61 

		//morpology ?�이�??�거 부�?
		int kernelSz = 2; //?�이�??�기
		int shape = MorphShapes::MORPH_RECT; //
		cv::Size sz = Size(2 * kernelSz + 1, 2 * kernelSz + 1); //?�lSz + 1) 
		Mat SE = cv::getStructuringElement(shape, sz); // 
		Mat src_open; //?�이즈�? ?�거???�태.
		int type = MorphTypes::MORPH_OPEN; // ?�이즈�? ?�거?�는 기능 MORPH_OPEN??type???�겠??
		cv::morphologyEx(binCrack, src_open, type, SE);// cv::morphologyEx(src_bin(?�력), src_open(출력), type, SE);//morphologyEx ?�이즈�? ?�거?�겠?? 

		cv::findContours(src_open, contours_crack, hierarchy_crack, RETR_TREE, CHAIN_APPROX_SIMPLE);

		string msg;
		msg = to_string(crack_num);

		if (contours_crack.empty())
		{
			// color = color_flawless;
		}

		else
		{
			for (size_t k = 0; k < contours_crack.size(); k++) //contourss.size();???�기만큼 for문을 ?�린??(?�롭�??�의??객체 ?�이�??? 
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
					cv::putText(drawColor, msg, vRois[i].br(), FONT_HERSHEY_SIMPLEX, 0.5, color, 1.5, 8);		//0, 255, 255
					cv::putText(drawColor, crack, vRois[i].tl(), FONT_HERSHEY_SIMPLEX, 0.5, color, 1.5, 8);
					cv::rectangle(drawColor, vRois[i], color, CV_FILLED);
					vRegions->push_back(vRois[i]); //crack ?�류 ?�각?�의 ?�이?��? ?�?? 

					/*int wid = (*vRegions)[i].width; : contamination ?�류�?거르�??�해 chip?�의 가�??�로 길이 ?�?�함
					int he = (*vRegions)[i].height;
					int checking = 0;*/

					/*cv::Point rtPts[4] = { vRegions[i].tl(), Point(vRegions[i].tl().x + vRegions[i].width, vRegions[i].tl().y),
					vRegions[i].br(), Point(vRegions[i].tl().x, vRegions[i].br().y) };
					for (size_t c = 0; c < 4; c++)
					{
						draw_line(drawColor, rtPts[c % 4], rtPts[(c + 1) % 4], color, 2, "", 3);
					}*/
				}
			}
		}

		int stop = 0;
	}

	return 1;
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
