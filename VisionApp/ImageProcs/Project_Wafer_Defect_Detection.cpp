#include "Common.h"
#include "Inspection.h"
#include "InspectContamination.h"

const string TEMPLATE = "./res/img/wafer_template.png";
const string FLAWLESS = "./res/img/final_real_wafer.png";
const string DONUT_BLACK = "./res/img/donut_black.png";
const string SCRATCH_BLACK = "./res/img/scratch_black.png";
const string LOCATION_BLACK = "./res/img/location_black.png";
const string EDGE_LOCATION_BLACK = "./res/img/edge_location_black.png";

const string DONUT_NAVY = "./res/img/Donut_navy.png";

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
	InspectContamination contamination;

	cv::Mat src = cv::imread(DONUT_BLACK, cv::ImreadModes::IMREAD_GRAYSCALE);
	cv::Mat drawColor = cv::imread(DONUT_BLACK, cv::ImreadModes::IMREAD_ANYCOLOR);

	contamination.OnTestProcess(src, drawColor, NULL);

	namedWindow("Contamination view", WINDOW_NORMAL);
	cv::resizeWindow("Contamination view", 1200, 800);
	imshow("Contamination view", drawColor);
	waitKey();

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
}