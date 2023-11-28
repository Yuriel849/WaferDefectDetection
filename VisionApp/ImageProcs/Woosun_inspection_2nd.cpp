// 예진 스크래치와 크랙 검출 함수 합본 코드 기타 환경에서 테스트용 
#pragma once
#include "Common.h"


std::string filePath_Search = "./res/img/final_real_wafer.png";
std::string filePath_Templt = "./res/img/wafer_template.png";

std::string filePath_Donut = "./res/img/donut_black.png";
std::string filePath_Edge_Location = "./res/img/edge_location_black.png";
std::string filePath_Location = "./res/img/location_black.png";
std::string filePath_Scratch = "./res/img/scratch_black.png";

// Below does not exist on this system (Yuriel)
std::string filePath_Scratch_Green = "../KCCImageNet/Wafer_detecting/real_end/Scratch_green.bmp";
std::string filePath_Location_Green = "../KCCImageNet/Wafer_detecting/real_end/Location_green.png";
std::string filePath_Edge_Location_Navy = "../KCCImageNet/Wafer_detecting/real_end/Edge_location_navy.png";
std::string filePath_Donut_Navy = "../KCCImageNet/Wafer_detecting/real_end/Donut_navy.png";


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


void draw_line(cv::Mat& img, cv::Point pt1, cv::Point pt2, cv::Scalar color, int thickness, std::string style, int gap)
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


vector<Rect> ScratchDetect(vector<Rect>& find, cv::Mat& search_img, cv::Mat& draw_img, cv::Mat& fill_img)
{
	// find			 MechingMethod()로 결함 없는 이미지를 템플릿 탐색하여 얻은, 올바른 좌표 배열 
	// search_img	 탐색할 이미지 IMREAD_GRAYSCALE
	// draw_img		 사각형 그릴 이미지 IMREAD_ANYCOLOR
	// fill_img	     색 채울 이미지 IMREAD_ANYCOLOR

	vector<Rect> find_scratch; // scratch 오류가 든 사각형 위치, 크기를 저장할 Rect형 vector

	RNG rng(12345);
	vector<vector<Point>> contours_scratch;
	vector<Vec4i> hierarchy_scratch;
	//Scalar color_flawless(255, 255, 0);
	Scalar color_scratch(0, 0, 255);

	int scratch_num = 1;
	string scratch = "Scratch";

	for (size_t i = 0; i < find.size(); i++)
	{
		Scalar color;
		Mat binScratch;
		Mat subImgScratch = search_img(find[i]).clone();

		//double thres_scratch = 210;
		cv::Point ptThreshold(817, 250);
		double thres_scratch = search_img.at<uchar>(ptThreshold.y, ptThreshold.x) + 60;
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
			for (size_t k = 0; k < contours_scratch.size(); k++) //contourss.size();의 크기만큼 for문을 돌린다.(새롭게 정의된 객체 사이즈(수) 
			{
				double area = contourArea(contours_scratch[k]);

				double area_min = 10 * 10;
				if (area_min <= area)
				{
					vector<vector<Point>> contours_scratch_error_all;
					vector<Point> contours_scratch_error;
					for (size_t p = 0; p < contours_scratch[k].size(); p++)
					{
						Point pt;
						pt.x = contours_scratch[k].at(p).x + find[i].x;
						pt.y = contours_scratch[k].at(p).y + find[i].y;
						contours_scratch_error.push_back(pt);
					}
					contours_scratch_error_all.push_back(contours_scratch_error);
					drawContours(draw_img, contours_scratch_error_all, 0, color_scratch, 3);



					color = color_scratch;
					scratch_num += 1;
					cv::putText(draw_img, msg, find[i].br(), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(255, 255, 0), 1.5, 8);		//0, 255, 255
					cv::putText(draw_img, scratch, find[i].tl(), FONT_HERSHEY_SIMPLEX, 0.5, color, 1.5, 8);
					cv::rectangle(fill_img, find[i], color, CV_FILLED);

					find_scratch.push_back(find[i]); //scratch 오류 사각형의 데이터를 저장. 

					cv::Point rtPts[4] = { find[i].tl(), Point(find[i].tl().x + find[i].width, find[i].tl().y),
					find[i].br(), Point(find[i].tl().x, find[i].br().y) };
					for (size_t c = 0; c < 4; c++)
					{
						draw_line(draw_img, rtPts[c % 4], rtPts[(c + 1) % 4], color, 2, "", 3);
					}
				}
			}

		}
	}
	return find_scratch;
}


vector<Rect> CrackDetect(vector<Rect>& find, cv::Mat& search_img, cv::Mat& draw_img, cv::Mat& fill_img)
{
	// find			 MechingMethod()로 결함 없는 이미지를 템플릿 탐색하여 얻은, 올바른 좌표 배열 
	// search_img	 탐색할 이미지 IMREAD_GRAYSCALE
	// draw_img		 사각형 그릴 이미지 IMREAD_ANYCOLOR
	// fill_img	     색 채울 이미지   IMREAD_ANYCOLOR

	vector<Rect> find_crack; // crack 오류가 든 사각형 위치, 크기를 저장할 Rect형 vector

	RNG rng(12345);
	vector<vector<Point>> contours_crack;
	vector<Vec4i> hierarchy_crack;
	Scalar color_flawless(0, 255, 0);
	Scalar color_crack(0, 255, 255);		// 255, 0, 0

	int crack_num = 1;
	string crack = "Crack";

	for (size_t i = 0; i < find.size(); i++)
	{
		Scalar color;
		Mat binCrack;
		Mat subImgCrack = search_img(find[i]).clone();

		cv::Point ptThreshold(885, 150);
		cv::drawMarker(draw_img, ptThreshold, CV_RGB(255, 255, 255), MarkerTypes::MARKER_CROSS);
		double thres_crack = search_img.at<uchar>(ptThreshold.y, ptThreshold.x) + 9;					//////////////////// 61
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
		msg = to_string(crack_num);

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
						pt.x = contours_crack[k].at(p).x + find[i].x;
						pt.y = contours_crack[k].at(p).y + find[i].y;
						contours_crack_error.push_back(pt);
					}
					contours_crack_error_all.push_back(contours_crack_error);
					drawContours(draw_img, contours_crack_error_all, 0, color_crack, 3);

					color = color_crack;
					crack_num += 1;
					cv::putText(draw_img, msg, find[i].br(), FONT_HERSHEY_SIMPLEX, 0.5, color, 1.5, 8);		//0, 255, 255
					cv::putText(draw_img, crack, find[i].tl(), FONT_HERSHEY_SIMPLEX, 0.5, color, 1.5, 8);
					cv::rectangle(fill_img, find[i], color, CV_FILLED);
					find_crack.push_back(find[i]); //crack 오류 사각형의 데이터를 저장. 


					cv::Point rtPts[4] = { find[i].tl(), Point(find[i].tl().x + find[i].width, find[i].tl().y),
					find[i].br(), Point(find[i].tl().x, find[i].br().y) };
					for (size_t c = 0; c < 4; c++)
					{
						draw_line(draw_img, rtPts[c % 4], rtPts[(c + 1) % 4], color, 2, "", 3);
					}
				}
			}
		}

		int stop = 0;
	}
	return find_crack;
}		//break point

vector<Rect> ContaminationDetect(vector<Rect>& find, cv::Mat& flawless_img, cv::Mat& search_img, cv::Mat& draw_img, cv::Mat& fill_img)
{
	//잘린 chip들 정보(Rect형) : find
	//비교할 멀쩡한 이미지(gray): flawless_img
	//비교할 이상한 이미지(gray) : search_img
	//검출할 오류를 그릴 이미지(color): draw_img
	//검출할 오류를 쉽게 볼 이미지(color): fill_img
	vector<Rect> find_contamination;

	Scalar color_con(255, 255, 121);

	vector<vector<Point>> contours_con; //하나의 chip 안에 큰 네모, 작은 네모 point들 확인할 contour 
	vector<Vec4i> hierarchy; //백터 hierarchy변수 생성
	vector<cv::Rect> vRoisLnS, vRois_Small, vRois_Large; //하나의 chip 안에 큰 네모, 작은 네모 각각의 벡터 
	Mat sub_img; //chip 하나 들어갈 이미지 (gray)
	bool  checkBig, checkSmall;
	for (size_t k = 0; k < find.size(); k++) //wafer 안의 chip 개수 만큼 for문 돌리기 
	{
		sub_img = search_img(find[k]).clone();
		Mat sub_img_draw;
		cv::cvtColor(sub_img, sub_img_draw, COLOR_GRAY2BGR); //chip 에 네모들 그릴 이미지 

		//큰 네모 색만 딸(이진화위해) 임계값 
		int inflate = 7;
		Point pt_big_subThrehold = Point(158, 12);
		double big_sub_threshold = sub_img.data[pt_big_subThrehold.y * sub_img.cols + pt_big_subThrehold.x] + 7;
		int check = 0;

		cv::threshold(sub_img, sub_img_draw, 130, 255, ThresholdTypes::THRESH_BINARY);
		cv::findContours(sub_img_draw, contours_con, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //객체 범위값을 구하기 위해 객체 외곽선 검출

		int check1 = 0;


		for (size_t k = 0; k < contours_con.size(); k++)
		{
			RotatedRect  rrt1 = minAreaRect(contours_con[k]);//객체 외곽선을 넘지 않을 만큼 의 사이즈를 wRrt에 담아줌
			Rect rt1 = rrt1.boundingRect(); // 객체의 위치에 상관없이 정방향 사각형 생성

			if (13 <= rt1.width && rt1.width <= 17 && 13 <= rt1.height && rt1.height <= 17)//small
			{
				vRois_Small.push_back(rt1); //작은 네모 담음
				cv::rectangle(sub_img, rrt1.boundingRect(), CV_RGB(0, 255, 0), 1); //green
			}
			if (18 <= rt1.width && rt1.width <= 22 && 18 <= rt1.height && rt1.height <= 22)//large
			{
				vRois_Large.push_back(rt1); //큰 네모 담음 
				cv::rectangle(sub_img, rrt1.boundingRect(), CV_RGB(0, 0, 255), 1); //blue
			}
			int check2 = 1;

		}
		int check3 = 1;
		checkBig = (vRois_Small.size() % 10 == 0);
		checkSmall = (vRois_Large.size() % 4 == 0);
		vRois_Small.clear();
		vRois_Large.clear();
		if (!(checkSmall && checkBig)) //small 10, big 4 
		{
			find_contamination.push_back(find[k]);
		}

	}

	return find_contamination;
}

Point pointTL(Rect rect)
{
	int pointXtl = rect.tl().x;
	int pointYtl = rect.tl().y;

	return Point(pointXtl, pointYtl);
}


int indexTL(Rect rect)
{
	int pointXtl = rect.tl().x;
	int pointYtl = rect.tl().y;
	int index = rect.tl().x + ((rect.tl().x + 1) * rect.tl().y);

	return index;
}

void main()
{
	cv::Mat src_flawless = cv::imread(filePath_Search, cv::ImreadModes::IMREAD_GRAYSCALE);		//비교할 완성품
	cv::Mat src_gray_templt = cv::imread(filePath_Templt, cv::ImreadModes::IMREAD_GRAYSCALE);	//템플릿
	cv::Mat src_gray_search = cv::imread(filePath_Donut, cv::ImreadModes::IMREAD_GRAYSCALE);	//
	cv::Mat src_draw = cv::imread(filePath_Donut, cv::ImreadModes::IMREAD_ANYCOLOR);	//
	cv::Mat src_filled = src_draw.clone();


	vector<Rect> scratch_error, crack_error, contamination_finish_not;
	vector<Rect> contamination_error;
	//vector<int> scratchTL, crackTL, conTL;
	vector<Point> scratchTL, crackTL, conTL;

	//find objects
	vector<cv::Rect> vRois;
	Mat obj_Region;
	Point ptThrehold = Point(888, 150);
	double min_threshold = src_gray_search.data[ptThrehold.y * src_gray_search.cols + ptThrehold.x] + 5;
	cv::threshold(src_gray_search, obj_Region, min_threshold, 255, ThresholdTypes::THRESH_BINARY);
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	cv::findContours(obj_Region, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //객체 범위값을 구하기 위해 객체 외곽선 검출
	for (size_t k = 0; k < contours.size(); k++) //contourss.size();의 크기만큼 for문을 돌린다.(새롭게 정의된 객체 사이즈(수) 
	{
		double area = contourArea(contours[k]);

		RotatedRect rrt = minAreaRect(contours[k]);//객체 외곽선을 넘지 않을 만큼 의 사이즈를 wRrt에 담아줌
		Rect rt = rrt.boundingRect(); // 객체의 위치에 상관없이 정방향 사각형 생성

		double area_min = 180 * 180 - 180 * 180 * 0.1;
		double area_max = 180 * 180 + 180 * 180 * 0.1;
		if (area_min <= area && area <= area_max)
		{

			vRois.push_back(rt);
			cv::rectangle(src_draw, rrt.boundingRect(), CV_RGB(0, 0, 255), 1);
		}
	}

	scratch_error = ScratchDetect(vRois, src_gray_search, src_draw, src_filled);
	for (int e = 0; e < scratch_error.size(); e++)
	{
		//scratchTL.push_back(indexTL(scratch_error[e])); 
		scratchTL.push_back(pointTL(scratch_error[e]));
	}
	//sort(scratchTL.begin(),scratchTL.end());

	crack_error = CrackDetect(vRois, src_gray_search, src_draw, src_filled);
	for (int w = 0; w < crack_error.size(); w++)
	{
		//crackTL.push_back(indexTL(crack_error[w])); 
		crackTL.push_back(pointTL(crack_error[w]));
	}
	//sort(crackTL.begin(), crackTL.end());


	contamination_error = ContaminationDetect(vRois, src_flawless, src_gray_search, src_draw, src_filled);
	for (int h = 0; h < contamination_error.size(); h++)
	{
		//conTL.push_back(indexTL(contamination_finish_not[h])); 
		conTL.push_back(pointTL(contamination_error[h]));
	}
	//sort(conTL.begin(), conTL.end());

	//scratch, crack 각각 비교 
	////////////////////////
	////////////////////////

	bool csFlag = false;

	for (size_t idx = 0; idx < contamination_error.size(); idx++)
	{
		for (size_t idxC = 0; idxC < crack_error.size(); idxC++)
		{
			int cX = crack_error.at(idxC).x;
			int cY = crack_error.at(idxC).y;
			if ((contamination_error.at(idx).x == cX) && (contamination_error.at(idx).y == cY))
			{
				csFlag = true;
				break;
			}
			int a = 0;
		}
		for (size_t idxS = 0; idxS < scratch_error.size(); idxS++)
		{
			int sX = scratch_error.at(idxS).x;
			int sY = scratch_error.at(idxS).y;
			if ((contamination_error.at(idx).x == sX) && (contamination_error.at(idx).y == sY))
			{
				csFlag = true;
				break;
			}
		}
		if (csFlag == true)
		{
			csFlag = false;
			contamination_error.erase(contamination_error.begin() + idx);
			idx--;
		}
	}

	//contamination_error:최종적으로 contamination error 담을 Rect형 벡터;




	Scalar color_con(255, 255, 121);
	string con = "Contamination";
	int con_num = 1;
	string msg;
	msg = to_string(con_num);

	for (int x = 0; x < contamination_error.size(); x++)
	{
		cv::putText(src_draw, msg, contamination_error[x].br(), FONT_HERSHEY_SIMPLEX, 0.5, color_con, 1.5, 8);		//0, 255, 255
		cv::putText(src_draw, con, contamination_error[x].tl(), FONT_HERSHEY_SIMPLEX, 0.5, color_con, 1.5, 8);
		cv::rectangle(src_filled, contamination_error[x], color_con, CV_FILLED);


		cv::Point rtPts[4] = { contamination_error[x].tl(), Point(contamination_error[x].tl().x + contamination_error[x].width, contamination_error[x].tl().y),
		contamination_error[x].br(), Point(contamination_error[x].tl().x, contamination_error[x].br().y) };
		for (size_t c = 0; c < contamination_error.size(); c++)
		{
			draw_line(src_draw, rtPts[c % 4], rtPts[(c + 1) % 4], color_con, 2, "", 3);
		}
	}

	int a1 = 0;

}