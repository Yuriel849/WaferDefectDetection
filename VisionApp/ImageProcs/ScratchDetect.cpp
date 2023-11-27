// 진짜. 최종. 수정본

#include "common.h"


void ScratchDetect(vector<Rect>& find, const Mat& search_img, const Mat& draw_img, const Mat& fill_img)
{
	// find			   MechingMethod()로 결함 없는 이미지를 템플릿 탐색하여 얻은, 올바른 좌표 배열 
  // search_img	 탐색할 이미지 IMREAD_GRAYSCALE
  // draw_img		 사각형 그릴 이미지 IMREAD_ANYCOLOR
  // fill_img	   색 채울 이미지   IMREAD_ANYCOLOR

	RNG rng(12345);
	vector<vector<Point>> contours_scratch;
	vector<Vec4i> hierarchy_scratch;
	Scalar color_flawless(0, 255, 0);
	Scalar color_scratch(0, 0, 255);

	int scratch_num = 1;
	string scratch = "Scratch";

	for (size_t i = 0; i < find.size(); i++)
	{
		Scalar color;
		Mat binScratch;
		Mat subImgScratch = search_img(find[i]).clone();

		double thres_scratch = 210;
		cv::threshold(subImgScratch, binScratch, thres_scratch, 255, ThresholdTypes::THRESH_BINARY);
		cv::findContours(binScratch, contours_scratch, hierarchy_scratch, RETR_TREE, CHAIN_APPROX_SIMPLE);

		string msg;
		msg = to_string(scratch_num);

		if (contours_scratch.empty())
		{
			// color = color_flawless;
		}

		else
		{
			if (contours_scratch[0].size() > 0)
			{
				color = color_scratch;
				scratch_num += 1;
				cv::putText(draw_img, msg, find[i].br(), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(255, 0, 255), 1.5, 8);
				cv::putText(draw_img, scratch, find[i].tl(), FONT_HERSHEY_SIMPLEX, 0.5, color, 1.5, 8);
				cv::rectangle(draw_img, find[i], color, 2);
				cv::drawMarker(draw_img, find[i].tl(), color, MarkerTypes::MARKER_CROSS);
				cv::rectangle(fill_img, find[i], color, CV_FILLED);
			}
		}
	}
}


void CrackDetect(vector<Rect>& find, const Mat& search_img, const Mat& draw_img, const Mat& fill_img)
{
	// find			   MechingMethod()로 결함 없는 이미지를 템플릿 탐색하여 얻은, 올바른 좌표 배열 
  // search_img	 탐색할 이미지 IMREAD_GRAYSCALE
  // draw_img		 사각형 그릴 이미지 IMREAD_ANYCOLOR
  // fill_img	   색 채울 이미지   IMREAD_ANYCOLOR

	RNG rng(12345);
	vector<vector<Point>> contours_crack;
	vector<Vec4i> hierarchy_crack;
	Scalar color_flawless(0, 255, 0);
	Scalar color_crack(255, 0, 0);

	int crack_num = 1;
	string crack = "Crack";

	for (size_t i = 0; i < find.size(); i++)
	{
		Scalar color;
		Mat binCrack;
		Mat subImgCrack = search_img(find[i]).clone();

		double thres_crack = 64;
		cv::threshold(subImgCrack, binCrack, thres_crack, 255, ThresholdTypes::THRESH_BINARY_INV);
		cv::findContours(binCrack, contours_crack, hierarchy_crack, RETR_TREE, CHAIN_APPROX_SIMPLE);

		string msg;
		msg = to_string(crack_num);

		if (contours_crack.empty())
		{
			// color = color_flawless;
		}

		else
		{
			if (contours_crack[0].size() > 0)
			{
				color = color_crack;
				crack_num += 1;
				cv::putText(draw_img, msg, find[i].br(), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(0, 255, 255), 1.5, 8);
				cv::putText(draw_img, crack, find[i].tl(), FONT_HERSHEY_SIMPLEX, 0.5, color, 1.5, 8);
				cv::rectangle(draw_img, find[i], color, 2);
				cv::drawMarker(draw_img, find[i].tl(), color, MarkerTypes::MARKER_CROSS);
				cv::rectangle(fill_img, find[i], color, CV_FILLED);
			}
		}
	}
}

void main()
{
	cv::Mat src_draw = cv::imread(filePath_Location, cv::ImreadModes::IMREAD_ANYCOLOR);
	cv::Mat src_flawless = cv::imread(filePath_Search, cv::ImreadModes::IMREAD_GRAYSCALE);
	cv::Mat src_gray_search = cv::imread(filePath_Location, cv::ImreadModes::IMREAD_GRAYSCALE);
	cv::Mat src_gray_templt = cv::imread(filePath_Templt, cv::ImreadModes::IMREAD_GRAYSCALE);
	cv::Mat src_filled = src_draw.clone();

	double thres = 0.999;
	vector<Rect> finds;
	MatchingMethod(src_flawless, src_gray_templt, thres, finds);

	CrackDetect(finds, src_gray_search, src_draw, src_filled);
	ScratchDetect(finds, src_gray_search, src_draw, src_filled);

	int a = 0;
}