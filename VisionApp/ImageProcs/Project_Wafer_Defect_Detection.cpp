#include "Common.h"

const string TEMPLATE = "./res/img/wafer_template.png";
const string FLAWLESS = "./res/img/final_real_wafer.png";
const string DONUT_BLACK = "./res/img/donut_black.png";
const string SCRATCH_BLACK = "./res/img/scratch_black.png";
const string LOCATION_BLACK = "./res/img/location_black.png";
const string EDGE_LOCATION_BLACK = "./res/img/edge_location_black.png";

Mat& mask(int boundary1, int boundary2, Mat& masked, Mat& target);

// Identical to function getROIs(const Mat& search_img, const Mat& ptrn_img, vector<Rect>& rois) in "23.ISP_Template_Matching_Img.cpp"
void MatchingMethod(const Mat& serch_img, const Mat& ptrn_img, const double& thres, vector<Rect>& rois)
{
	Mat result = Mat::zeros(Size(serch_img.cols - ptrn_img.cols + 1, serch_img.rows - ptrn_img.rows + 1), CV_32FC1);
	int match_method = TM_CCORR_NORMED;
	matchTemplate(serch_img, ptrn_img, result, match_method);
	normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

	double minVal; double maxVal; Point minLoc; Point maxLoc;
	Point matchLoc;

	bool multiobjects = true;
	if (!multiobjects)
	{
		minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
		if (match_method == TM_SQDIFF || match_method == TM_SQDIFF_NORMED)
		{
			matchLoc = minLoc;
		}
		else
		{
			matchLoc = maxLoc;
		}

		rois.push_back(Rect(matchLoc.x, matchLoc.y, ptrn_img.cols, ptrn_img.rows));
	}
	else
	{
		Mat tm_bin;
		double min_thr = thres;
		double max_thr = 1.0;
		cv::threshold(result, tm_bin, min_thr, max_thr, ThresholdTypes::THRESH_BINARY);
		tm_bin *= 255;

		tm_bin.convertTo(tm_bin, CV_8UC1);

		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;
		findContours(tm_bin, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

		for (size_t i = 0; i < contours.size(); i++)
		{
			RotatedRect rrt = minAreaRect(contours[i]);
			//rois.push_back(rrt.boundingRect());
			rois.push_back(Rect(rrt.boundingRect().tl().x, rrt.boundingRect().tl().y, ptrn_img.cols, ptrn_img.rows));
		}
	}
}

void main()
{
	//inspection :: scratch
	//각 class 만들어서
	//해당 클래스 인스턴스를 이용하여 검사를 수행한다.

	return;



	// -- Scratch detection
	//cv::Mat src_draw = cv::imread(Donut_Black, cv::ImreadModes::IMREAD_ANYCOLOR);	// 지금 읽을 이미지 
	//cv::Mat src_flawless = cv::imread(Flawless, cv::ImreadModes::IMREAD_GRAYSCALE);
	//cv::Mat src_gray_search = cv::imread(Donut_Black, cv::ImreadModes::IMREAD_GRAYSCALE);
	//cv::Mat src_gray_templt = cv::imread(Template, cv::ImreadModes::IMREAD_GRAYSCALE);
	//cv::Mat src_filled_flawless = src_draw.clone();

	//double thres = 0.999;
	//vector<Rect> finds;

	//MatchingMethod(src_flawless, src_gray_templt, thres, finds);

	//RNG rng(12345);
	//vector<vector<Point>> contours;
	//vector<Vec4i> hierarchy;
	//Scalar judge_color(0, 255, 0);
	//Scalar judge_color_NG(0, 0, 255);

	//for (size_t i = 0; i < finds.size(); i++)
	//{
	//	Scalar color;
	//	Mat subImg = src_gray_search(finds[i]).clone();

	//	double threshold_bin = 210;
	//	Mat binScratch;
	//	cv::threshold(subImg, binScratch, threshold_bin, 255, ThresholdTypes::THRESH_BINARY);

	//	findContours(binScratch, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

	//	if (contours.empty())
	//	{
	//		color = judge_color;
	//	}

	//	else
	//	{
	//		if (contours[0].size() > 0)
	//		{
	//			color = judge_color_NG;
	//		}
	//	}

	//	cv::rectangle(src_draw, finds[i], CV_RGB(255, 0, 0), 2);
	//	string msg;
	//	cv::drawMarker(src_draw, finds[i].tl(), CV_RGB(255, 0, 0), MarkerTypes::MARKER_CROSS);
	//	msg = to_string(i + 1);
	//	putText(src_draw, msg, finds[i].tl(), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(0, 255, 255), 2, 8);
	//	cv::rectangle(src_filled_flawless, finds[i], color, CV_FILLED);

	//	int z = 0;
	//}

	//int a = 0;

	// -- Crack detection


	//std::string filePath_Search = "./res/img/final_real_wafer.png";
	//std::string filePath_Templt = "./res/img/wafer_template.png";
	//std::string filePath_Donut = "./res/img/donut_black.png";
	//std::string filePath_Scratch = "./res/img/scratch_black.png";
	//std::string filePath_EL = "./res/img/edge_location_black.png";
	//std::string filePath_Location = "./res/img/location_black.png";

	//cv::Mat src_gray_search = cv::imread(filePath_Search, cv::ImreadModes::IMREAD_GRAYSCALE);
	//cv::Mat src_gray_templt = cv::imread(filePath_Templt, cv::ImreadModes::IMREAD_GRAYSCALE);

	//cv::Mat src_gray_donut = cv::imread(filePath_Donut, cv::ImreadModes::IMREAD_GRAYSCALE);
	//cv::Mat src_gray_scratch = cv::imread(filePath_Scratch, cv::ImreadModes::IMREAD_GRAYSCALE);
	//cv::Mat src_gray_el = cv::imread(filePath_EL, cv::ImreadModes::IMREAD_GRAYSCALE);
	//cv::Mat src_gray_location = cv::imread(filePath_Location, cv::ImreadModes::IMREAD_GRAYSCALE);

	//cv::Mat src_draw = cv::imread(filePath_Location, cv::ImreadModes::IMREAD_ANYCOLOR);

	//cv::Mat src_filled = src_draw.clone();

	//double thres = 0.999;
	//vector<Rect> finds;

	////완성품 
	//MatchingMethod(src_gray_search, src_gray_templt, thres, finds);// ROIs
	//RNG rng(12345);

	//vector<vector<Point>> contours;
	//vector<Vec4i> hierarchy;

	//Scalar judge_color_OK(255, 0, 0);
	//Scalar judge_color_NG(0, 0, 255);

	//for (size_t k = 0; k < finds.size(); k++)
	//{
	//	Scalar color;

	//	Mat subImg = src_gray_location(finds[k]).clone();
	//	double threshold_bin = 64;
	//	Mat binCrack;
	//	cv::threshold(subImg, binCrack, threshold_bin, 255, ThresholdTypes::THRESH_BINARY_INV);

	//	findContours(binCrack, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

	//	if (contours.empty())
	//	{
	//		color = judge_color_OK;
	//	}
	//	else {
	//		if (contours[0].size() > 0)
	//		{
	//			color = judge_color_NG;
	//			//drawContours(src_draw, contours, 0, color, 1);
	//		}
	//	}

	//	cv::rectangle(src_draw, finds[k], color, 1);
	//	string msg;
	//	cv::drawMarker(src_draw, finds[k].tl(), color, MarkerTypes::MARKER_CROSS);
	//	msg = to_string(k + 1);
	//	putText(src_draw, msg, finds[k].tl(), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(0, 255, 255), 1, 8);

	//	cv::rectangle(src_filled, finds[k], color, CV_FILLED);

	//	int a = 0;
	//}
	//int v = 0;



	// -- Chip invasion detection

	bool defectFlag = false;

	cv::Mat src_gray_search = cv::imread(DONUT_BLACK, cv::ImreadModes::IMREAD_GRAYSCALE);
	cv::Mat src_gray_templt = cv::imread(TEMPLATE, cv::ImreadModes::IMREAD_GRAYSCALE);

	double thres = 0.99;
	vector<Rect> finds;
	MatchingMethod(src_gray_search, src_gray_templt, thres, finds);



	cv::Mat src_draw = cv::imread(DONUT_BLACK, cv::ImreadModes::IMREAD_ANYCOLOR);
	cv::Mat src_filled = src_draw.clone();

	for (size_t k = 0; k < finds.size(); k++)
	{
		cv::rectangle(src_draw, finds[k], CV_RGB(255, 0, 0), 1);
		cv::drawMarker(src_draw, finds[k].tl(), CV_RGB(255, 0, 0), MarkerTypes::MARKER_CROSS);
		string msg = to_string(k + 1);
		putText(src_draw, msg, finds[k].tl(), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(0, 255, 255), 1, 8);
		cv::rectangle(src_filled, finds[k], CV_RGB(0, 205, 255), CV_FILLED);

		//---- Check this wafer for chip invasion defects ----//
		cv::Mat src_templt = src_gray_templt.clone();
		cv::Mat src_wafer = src_gray_search(finds[k]).clone();
		cv::Mat src_wMasked = mask(28, 112, src_wMasked, src_wafer); // For wafer : Mask y-axis 28 - 112
		cv::Mat src_tMasked = mask(24, 106, src_tMasked, src_templt); // For template : Mask y-axis 24 - 106
		// Binarization
		cv::Mat src_tBin = cv::Mat::zeros(src_templt.size(), CV_8UC1);
		cv::Mat src_wBin = cv::Mat::zeros(src_wafer.size(), CV_8UC1);
		double thres = 125;
		cv::threshold(src_wMasked, src_wBin, thres, 255, ThresholdTypes::THRESH_BINARY);
		cv::threshold(src_tMasked, src_tBin, thres, 255, ThresholdTypes::THRESH_BINARY);
		//Morphology
		int kernelSz = 1;
		int shape = MorphShapes::MORPH_ELLIPSE;
		cv::Size sz = Size(2 * kernelSz + 1, 2 * kernelSz + 1);
		Mat SE = cv::getStructuringElement(shape, sz);
		cv::Mat src_wErode, src_tErode;
		cv::erode(src_wBin, src_wErode, SE);
		cv::erode(src_tBin, src_tErode, SE);
		// Find contours
		vector<vector<Point>> wContours, tContours;
		vector<Vec4i> wHierarchy, tHierarchy;
		findContours(src_wErode, wContours, wHierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		findContours(src_tErode, tContours, tHierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		Mat drawing_wafer = src_wafer.clone();
		Mat drawing_templt = src_templt.clone();
		// Check area of bounding rectangles
		if (wContours.size() == tContours.size())
		{
			for (size_t i = 0; i < wContours.size(); i++)
			{
				Rect2f wR2f = minAreaRect(wContours[i]).boundingRect2f();
				Rect2f tR2f = minAreaRect(tContours[i]).boundingRect2f();
				// Identify squares with minimum area bounding rectangle
				cv::rectangle(drawing_wafer, wR2f.tl(), wR2f.br(), 0, 1, LINE_8);
				cv::rectangle(drawing_templt, tR2f.tl(), tR2f.br(), 0, 1, LINE_8);
				// If difference in area greater than 30
				if (abs(tR2f.area() - wR2f.area()) > 30) { defectFlag = true; break; }
			}
		}

		if (defectFlag)
		{
			cv::rectangle(src_filled, finds[k], CV_RGB(255, 0, 100), CV_FILLED);
			defectFlag = false;
		}
		int b = 0;
	}
	int a = 0;
}

Mat& mask(int boundaryLow, int boundaryHigh, Mat& masked, Mat& target)
{
	cv::Mat mask = cv::Mat::zeros(target.size(), CV_8UC1);
	uchar* mData = mask.data;
	for (size_t row = 0; row < target.rows; row++)
	{
		for (size_t col = 0; col < target.cols; col++)
		{
			int index = row * target.cols + col;
			if (!(boundaryLow < row && row < boundaryHigh)) { mData[index] = 255; }
		}
	}
	masked = target & mask;
	return masked;
}