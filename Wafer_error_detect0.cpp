
// ���� ��ũ��ġ�� ũ�� ���� �Լ� �պ� �ڵ� ��Ÿ ȯ�濡�� �׽�Ʈ�� 
#pragma once
#include "Common.h"


std::string filePath_Search = "../KCCImageNet/Wafer_detecting/final_real_wafer.png";
std::string filePath_Templt = "../KCCImageNet/Wafer_detecting/wafer_template.png";

std::string filePath_Donut = "../KCCImageNet/Wafer_detecting/donut_black.png";
std::string filePath_Edge_Location = "../KCCImageNet/Wafer_detecting/edge_location_black.png";
std::string filePath_Location = "../KCCImageNet/Wafer_detecting/location_black.png";
std::string filePath_Scratch = "../KCCImageNet/Wafer_detecting/scratch_black.png";

std::string filePath_Scratch_Green = "../KCCImageNet/Wafer_detecting/Scratch_green.png";
std::string filePath_Location_Green = "../KCCImageNet/Wafer_detecting/Location_green.png";
std::string filePath_Edge_Location_Navy = "../KCCImageNet/Wafer_detecting/Edge_location_navy.png";
std::string filePath_Donut_Navy = "../KCCImageNet/Wafer_detecting/Donut_navy.png";


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


void ScratchDetect(vector<Rect>& find, cv::Mat& search_img, cv::Mat& draw_img, cv::Mat& fill_img)
{
	// find			 MechingMethod()�� ���� ���� �̹����� ���ø� Ž���Ͽ� ����, �ùٸ� ��ǥ �迭 
	// search_img	 Ž���� �̹��� IMREAD_GRAYSCALE
	// draw_img		 �簢�� �׸� �̹��� IMREAD_ANYCOLOR
	// fill_img	     �� ä�� �̹��� IMREAD_ANYCOLOR

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
			for (size_t k = 0; k < contours_scratch.size(); k++) //contourss.size();�� ũ�⸸ŭ for���� ������.(���Ӱ� ���ǵ� ��ü ������(��) 
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
}


void CrackDetect(vector<Rect>& find, cv::Mat& search_img, cv::Mat& draw_img, cv::Mat& fill_img)
{
	// find			 MechingMethod()�� ���� ���� �̹����� ���ø� Ž���Ͽ� ����, �ùٸ� ��ǥ �迭 
	// search_img	 Ž���� �̹��� IMREAD_GRAYSCALE
	// draw_img		 �簢�� �׸� �̹��� IMREAD_ANYCOLOR
	// fill_img	     �� ä�� �̹���   IMREAD_ANYCOLOR

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
		cv::threshold(subImgCrack, binCrack, thres_crack, 255, ThresholdTypes::THRESH_BINARY_INV);	//INV -> 43���� ��ο� �� + 61 

		//morpology ������ ���� �κ�
		int kernelSz = 2; //������ ũ��
		int shape = MorphShapes::MORPH_RECT; //
		cv::Size sz = Size(2 * kernelSz + 1, 2 * kernelSz + 1); //��lSz + 1) 
		Mat SE = cv::getStructuringElement(shape, sz); // 
		Mat src_open; //����� ���ŵ� ����.
		int type = MorphTypes::MORPH_OPEN; // ����� �����ϴ� ��� MORPH_OPEN�� type�� ��ڴ�.
		cv::morphologyEx(binCrack, src_open, type, SE);// cv::morphologyEx(src_bin(�Է�), src_open(���), type, SE);//morphologyEx ����� �����ϰڴ�. 

		cv::findContours(src_open, contours_crack, hierarchy_crack, RETR_TREE, CHAIN_APPROX_SIMPLE);

		string msg;
		msg = to_string(crack_num);

		if (contours_crack.empty())
		{
			// color = color_flawless;
		}

		else
		{
			for (size_t k = 0; k < contours_crack.size(); k++) //contourss.size();�� ũ�⸸ŭ for���� ������.(���Ӱ� ���ǵ� ��ü ������(��) 
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
}		//break point

void ContaminationDetect(vector<Rect>& find, cv::Mat& flawless_img, cv::Mat& search_img, cv::Mat& draw_img, cv::Mat& fill_img)
{
	Mat draw_Pads;
	cv::cvtColor(flawless_img, draw_Pads, COLOR_GRAY2BGR);

	Mat Pads_Region;
	double obj_PAD_threshold = 130;
	cv::threshold(flawless_img, Pads_Region, obj_PAD_threshold, 255, ThresholdTypes::THRESH_BINARY);
	vector<vector<Point>> contours; //���Ӱ� �������� ���ϱ� ���� ���� ����Ʈ ���� contourss ���� 
	vector<Vec4i> hierarchy; //���� hierarchy���� ����
	cv::findContours(Pads_Region, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //��ü �������� ���ϱ� ���� ��ü �ܰ��� ����
	
	RotatedRect rrt1,rrt2; 
	Rect rt1,rt2,rt3;	 

	vector<cv::Rect> vRois_Small, vRois_Large;
	for (size_t k = 0; k < contours.size(); k++)
	{
		rrt1 = minAreaRect(contours[k]);//��ü �ܰ����� ���� ���� ��ŭ �� ����� wRrt�� �����
		rt1 = rrt1.boundingRect(); // ��ü�� ��ġ�� ������� ������ �簢�� ����

		if (13 <= rt1.width && rt1.width <= 17)//small
		{
			vRois_Small.push_back(rt1);
			cv::rectangle(draw_Pads, rrt1.boundingRect(), CV_RGB(0, 255, 0), 1); //src_openn���� ã�Ƴ� ��ü�� �ܰ��� �κ��� wR2f.tl() �»� ��������
		}
		if (18 <= rt1.width && rt1.width <= 22)//large
		{
			vRois_Large.push_back(rt1);
			cv::rectangle(draw_Pads, rrt1.boundingRect(), CV_RGB(0, 0, 255), 1); //src_openn���� ã�Ƴ� ��ü�� �ܰ��� �κ��� wR2f.tl() �»� ��������
		}
	}

	//find objects
	vector<cv::Rect> vRois;
	Mat obj_Region;
	Point ptThrehold = Point(888, 150);
	double min_threshold = search_img.data[ptThrehold.y * search_img.cols + ptThrehold.x] + 5;
	cv::threshold(search_img, obj_Region, min_threshold, 255, ThresholdTypes::THRESH_BINARY);
	contours.clear(); //���Ӱ� �������� ���ϱ� ���� ���� ����Ʈ ���� contourss ���� 
	hierarchy.clear(); //���� hierarchyy���� ����
	cv::findContours(obj_Region, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //��ü �������� ���ϱ� ���� ��ü �ܰ��� ����
	for (size_t k = 0; k < contours.size(); k++) //contourss.size();�� ũ�⸸ŭ for���� ������.(���Ӱ� ���ǵ� ��ü ������(��) 
	{
		double area = contourArea(contours[k]);

		rrt2 = minAreaRect(contours[k]);//��ü �ܰ����� ���� ���� ��ŭ �� ����� wRrt�� �����
		rt2 = rrt2.boundingRect(); // ��ü�� ��ġ�� ������� ������ �簢�� ����

		if (160 <= rt2.width && rt2.width <= 180)
		{
			vRois.push_back(rt2);
			cv::rectangle(draw_img, rrt2.boundingRect(), CV_RGB(255, 0, 0), 1); //src_openn���� ã�Ƴ� ��ü�� �ܰ��� �κ��� wR2f.tl() �»� ��������
		}
	}


	//sub chips
	vector<cv::Rect> vRois_Small_Err, vRois_Large_Err;
	for (size_t k = 0; k < vRois.size(); k++) //contourss.size();�� ũ�⸸ŭ for���� ������.(���Ӱ� ���ǵ� ��ü ������(��) 
	{
		Mat sub_img = search_img(vRois[k]).clone();
		Mat sub_img_draw;
		cv::cvtColor(sub_img, sub_img_draw, COLOR_GRAY2BGR);
		//large pad
		int inflate = 7;
		for (size_t i = 0; i < vRois_Large.size(); i++)
		{
			rt3 = vRois_Large[i];
			rt3.x -= inflate;
			rt3.width += inflate * 2;
			rt3.height += inflate * 2;

			cv::rectangle(sub_img_draw, rt3, CV_RGB(255, 255, 0), 1);

			Mat Pad = sub_img(rt3).clone(); ///����ó������ ���� 
			Mat Pad_Bin;
			cv::threshold(Pad, Pad_Bin, obj_PAD_threshold, 255, ThresholdTypes::THRESH_BINARY);

			contours.clear(); //���Ӱ� �������� ���ϱ� ���� ���� ����Ʈ ���� contourss ���� 
			hierarchy.clear(); //���� hierarchyy���� ����
			cv::findContours(Pad_Bin, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //��ü �������� ���ϱ� ���� ��ü �ܰ��� ����
			for (size_t p = 0; p < contours.size(); p++) //contourss.size();�� ũ�⸸ŭ for���� ������.(���Ӱ� ���ǵ� ��ü ������(��) 
			{
				double area = contourArea(contours[p]);
				if (area < 10) continue;
				RotatedRect rrt = minAreaRect(contours[p]);//��ü �ܰ����� ���� ���� ��ŭ �� ����� wRrt�� �����
				Rect rt = rrt.boundingRect(); // ��ü�� ��ġ�� ������� ������ �簢�� ����

				if (18 <= rt.width && rt.width <= 22)//large
				{

				}
				else
				{
					Rect rtSubErrRgn = rt;
					rtSubErrRgn.x += vRois_Large[i].x - inflate;
					rtSubErrRgn.y += vRois_Large[i].y;
					cv::rectangle(sub_img_draw, rtSubErrRgn, CV_RGB(0, 255, 255), 1); //src_openn���� ã�Ƴ� ��ü�� �ܰ��� �κ��� wR2f.tl() �»� ��������

					Rect rtDraw = rt;
					rtDraw.x += vRois_Large[i].x + vRois[k].x - inflate;
					rtDraw.y += vRois_Large[i].y + vRois[k].y;
					vRois_Large_Err.push_back(rtDraw);
					//cv::rectangle(sub_img_draw, rrt.boundingRect(), CV_RGB(255, 0, 0), 1); //src_openn���� ã�Ƴ� ��ü�� �ܰ��� �κ��� wR2f.tl() �»� ��������
				}
			}
		}
		//small pad	
		for (size_t i = 0; i < vRois_Small.size(); i++)
		{
			Rect rt = vRois_Small[i];
			rt.x -= inflate;
			rt.y -= inflate;
			rt.width += inflate * 2;
			rt.height += inflate + 2;

			cv::rectangle(sub_img_draw, rt, CV_RGB(255, 255, 0), 1); //src_openn���� ã�Ƴ� ��ü�� �ܰ��� �κ��� wR2f.tl() �»� ��������

			Mat Pad = sub_img(rt).clone();
			Mat Pad_Bin;
			cv::threshold(Pad, Pad_Bin, obj_PAD_threshold, 255, ThresholdTypes::THRESH_BINARY);

			contours.clear(); //���Ӱ� �������� ���ϱ� ���� ���� ����Ʈ ���� contourss ���� 
			hierarchy.clear(); //���� hierarchyy���� ����
			cv::findContours(Pad_Bin, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //��ü �������� ���ϱ� ���� ��ü �ܰ��� ����
			for (size_t p = 0; p < contours.size(); p++) //contourss.size();�� ũ�⸸ŭ for���� ������.(���Ӱ� ���ǵ� ��ü ������(��) 
			{
				double area = contourArea(contours[p]);
				if (area < 10) continue;
				RotatedRect rrt = minAreaRect(contours[p]);//��ü �ܰ����� ���� ���� ��ŭ �� ����� wRrt�� �����
				Rect rt = rrt.boundingRect(); // ��ü�� ��ġ�� ������� ������ �簢�� ����

				if (13 <= rt.width && rt.width <= 17)//small
				{
				}
				else {
					Rect rtSubErrRgn = rt;
					rtSubErrRgn.x += vRois_Small[i].x - inflate;
					rtSubErrRgn.y += vRois_Small[i].y;
					cv::rectangle(sub_img_draw, rtSubErrRgn, CV_RGB(0, 255, 255), 1); //src_openn���� ã�Ƴ� ��ü�� �ܰ��� �κ��� wR2f.tl() �»� ��������

					cv::rectangle(sub_img_draw, rtSubErrRgn, CV_RGB(0, 255, 255), 1); //src_openn���� ã�Ƴ� ��ü�� �ܰ��� �κ��� wR2f.tl() �»� ��������
					Rect rtDraw = rt;//chips
					rtDraw.x += vRois_Small[i].x + vRois[k].x - inflate;
					rtDraw.y += vRois_Small[i].y + vRois[k].y;
					vRois_Small_Err.push_back(rtDraw);
					//cv::rectangle(sub_img_draw, rrt.boundingRect(), CV_RGB(255, 0, 0), 1); //src_openn���� ã�Ƴ� ��ü�� �ܰ��� �κ��� wR2f.tl() �»� ��������
				}
			}

		}
	}

	//display result :: error rectangle
	for (size_t i = 0; i < vRois_Large_Err.size(); i++)
	{
		cv::rectangle(draw_img, vRois_Large_Err[i], CV_RGB(255, 0, 255), 1); //src_openn���� ã�Ƴ� ��ü�� �ܰ��� �κ��� wR2f.tl() �»� ��������
	}
	for (size_t i = 0; i < vRois_Small_Err.size(); i++)
	{
		cv::rectangle(draw_img, vRois_Small_Err[i], CV_RGB(255, 0, 255), 1); //src_openn���� ã�Ƴ� ��ü�� �ܰ��� �κ��� wR2f.tl() �»� ��������
	}
}




void main()
{
	cv::Mat src_flawless = cv::imread(filePath_Search, cv::ImreadModes::IMREAD_GRAYSCALE);		//���� �ϼ�ǰ
	cv::Mat src_gray_templt = cv::imread(filePath_Templt, cv::ImreadModes::IMREAD_GRAYSCALE);	//���ø�
	cv::Mat src_gray_search = cv::imread(filePath_Donut_Navy, cv::ImreadModes::IMREAD_GRAYSCALE);	//
	cv::Mat src_draw = cv::imread(filePath_Donut_Navy, cv::ImreadModes::IMREAD_ANYCOLOR);	//
	cv::Mat src_filled = src_draw.clone();

	{
		//find objects
		vector<cv::Rect> vRois;
		Mat obj_Region;
		Point ptThrehold = Point(888, 150);
		double min_threshold = src_gray_search.data[ptThrehold.y * src_gray_search.cols + ptThrehold.x] + 5;
		cv::threshold(src_gray_search, obj_Region, min_threshold, 255, ThresholdTypes::THRESH_BINARY);
		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;
		cv::findContours(obj_Region, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //��ü �������� ���ϱ� ���� ��ü �ܰ��� ����
		for (size_t k = 0; k < contours.size(); k++) //contourss.size();�� ũ�⸸ŭ for���� ������.(���Ӱ� ���ǵ� ��ü ������(��) 
		{
			double area = contourArea(contours[k]);

			RotatedRect rrt = minAreaRect(contours[k]);//��ü �ܰ����� ���� ���� ��ŭ �� ����� wRrt�� �����
			Rect rt = rrt.boundingRect(); // ��ü�� ��ġ�� ������� ������ �簢�� ����

			double area_min = 180 * 180 - 180 * 180 * 0.1;
			double area_max = 180 * 180 + 180 * 180 * 0.1;
			if (area_min <= area && area <= area_max)
			{

				vRois.push_back(rt);
				cv::rectangle(src_draw, rrt.boundingRect(), CV_RGB(0, 0, 255), 1); //src_openn���� ã�Ƴ� ��ü�� �ܰ��� �κ��� wR2f.tl() �»� ��������
			}
		}

		ScratchDetect(vRois, src_gray_search, src_draw, src_filled);
		CrackDetect(vRois, src_gray_search, src_draw, src_filled);
		ContaminationDetect(vRois, src_flawless, src_gray_search, src_draw, src_filled);
	}

	int a = 0;
}

