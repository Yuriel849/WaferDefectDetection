
//        ũ  ġ   ũ         Լ   պ   ڵ    Ÿ ȯ 濡    ׽ Ʈ   
#pragma once
#include "Common.h"


std::string filePath_Search = "./res/img/final_real_wafer.png";
std::string filePath_Templt = "./res/img/wafer_template.png";

std::string filePath_Donut = "./res/img/donut_black.png";
std::string filePath_Edge_Location = "./res/img/edge_location_black.png";
std::string filePath_Location = "./res/img/location_black.png";
std::string filePath_Scratch = "./res/img/scratch_black.png";

std::string filePath_Scratch_Green = "./res/img/Scratch_green.bmp";
std::string filePath_Location_Green = "./res/img/Location_green.png";
std::string filePath_Edge_Location_Navy = "./res/img/Edge_location_navy.png";
std::string filePath_Donut_Navy = "./res/img/Donut_navy.png";


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
	// find			 MechingMethod()              ̹         ø  Ž   Ͽ      ,  ùٸ    ǥ  迭 
	// search_img	 Ž      ̹    IMREAD_GRAYSCALE
	// draw_img		  簢    ׸   ̹    IMREAD_ANYCOLOR
	// fill_img	        ä    ̹    IMREAD_ANYCOLOR

	vector<Rect> find_scratch; // scratch            簢     ġ, ũ ⸦        Rect   vector

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
			for (size_t k = 0; k < contours_scratch.size(); k++) //contourss.size();   ũ ⸸ŭ for           .(   Ӱ     ǵ    ü       (  ) 
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

					find_scratch.push_back(find[i]); //scratch       簢          ͸      . 

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
	// find			 MechingMethod()              ̹         ø  Ž   Ͽ      ,  ùٸ    ǥ  迭 
	// search_img	 Ž      ̹    IMREAD_GRAYSCALE
	// draw_img		  簢    ׸   ̹    IMREAD_ANYCOLOR
	// fill_img	        ä    ̹      IMREAD_ANYCOLOR

	vector<Rect> find_crack; // crack            簢     ġ, ũ ⸦        Rect   vector

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
		cv::threshold(subImgCrack, binCrack, thres_crack, 255, ThresholdTypes::THRESH_BINARY_INV);	//INV -> 43       ο     + 61 

		//morpology              κ 
		int kernelSz = 2; //       ũ  
		int shape = MorphShapes::MORPH_RECT; //
		cv::Size sz = Size(2 * kernelSz + 1, 2 * kernelSz + 1); //  lSz + 1) 
		Mat SE = cv::getStructuringElement(shape, sz); // 
		Mat src_open; //         ŵ      .
		int type = MorphTypes::MORPH_OPEN; //            ϴ      MORPH_OPEN   type     ڴ .
		cv::morphologyEx(binCrack, src_open, type, SE);// cv::morphologyEx(src_bin( Է ), src_open(   ), type, SE);//morphologyEx            ϰڴ . 

		cv::findContours(src_open, contours_crack, hierarchy_crack, RETR_TREE, CHAIN_APPROX_SIMPLE);

		string msg;
		msg = to_string(crack_num);

		if (contours_crack.empty())
		{
			// color = color_flawless;
		}

		else
		{
			for (size_t k = 0; k < contours_crack.size(); k++) //contourss.size();   ũ ⸸ŭ for           .(   Ӱ     ǵ    ü       (  ) 
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
					find_crack.push_back(find[i]); //crack       簢          ͸      . 
					int wid = find[i].width;
					int he = find[i].height;
					int checking = 0;

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
	// ߸  chip       (Rect  ) : find
	//             ̹   (gray): flawless_img
	//      ̻     ̹   (gray) : search_img
	//               ׸   ̹   (color): draw_img
	//                       ̹   (color): fill_img
	vector<Rect> find_contamination;

	Scalar color_con(255, 255, 121);

	vector<vector<Point>> contours_con; // ϳ    chip  ȿ  ū  ׸ ,       ׸  point   Ȯ     contour 
	vector<Vec4i> hierarchy_con; //     hierarchy         
	vector<cv::Rect> vRoisLnS, vRois_Small, vRois_Large; // ϳ    chip  ȿ  ū  ׸ ,       ׸              
	Mat sub_img; //chip  ϳ      ̹    (gray)
	bool  checkBig, checkSmall;

	for (size_t k = 0; k < find.size(); k++) //wafer      chip        ŭ for          
	{
		sub_img = search_img(find[k]).clone();
		Mat sub_img_draw;
		cv::cvtColor(sub_img, sub_img_draw, COLOR_GRAY2BGR); //chip     ׸    ׸   ̹    

		//ū  ׸         (    ȭ    )  Ӱ谪 
		int inflate = 7;
		Point pt_big_subThrehold = Point(158, 12);
		double big_sub_threshold = sub_img.data[pt_big_subThrehold.y * sub_img.cols + pt_big_subThrehold.x] + 7;
		int check = 0;

		cv::threshold(sub_img, sub_img_draw, 130, 255, ThresholdTypes::THRESH_BINARY);

		//morpology              κ 
		int kernelSz = 2; //       ũ  
		int shape = MorphShapes::MORPH_RECT; //
		cv::Size sz = Size(2 * kernelSz + 1, 2 * kernelSz + 1); //  lSz + 1) 
		Mat SE = cv::getStructuringElement(shape, sz); // 
		Mat src_open; //         ŵ      .
		int type = MorphTypes::MORPH_OPEN; //            ϴ      MORPH_OPEN   type     ڴ .
		cv::morphologyEx(sub_img_draw, src_open, type, SE);// cv::morphologyEx(src_bin( Է ), src_open(   ), type, SE);//morphologyEx            ϰڴ . 

		cv::findContours(src_open, contours_con, hierarchy_con, RETR_TREE, CHAIN_APPROX_SIMPLE);

		//cv::findContours(sub_img_draw, contours_con, hierarchy_con, RETR_TREE, CHAIN_APPROX_SIMPLE); //  ü             ϱ         ü  ܰ        

		int check1 = 0;


		for (size_t k = 0; k < contours_con.size(); k++)
		{
			RotatedRect  rrt1 = minAreaRect(contours_con[k]);//  ü  ܰ                  ŭ          wRrt        
			Rect rt1 = rrt1.boundingRect(); //   ü     ġ                   簢       

			if ((13 <= rt1.width && rt1.width <= 17) && (13 <= rt1.height && rt1.height <= 17))//small
			{
				vRois_Small.push_back(rt1); //      ׸      
				cv::rectangle(sub_img, rrt1.boundingRect(), CV_RGB(0, 255, 0), 1); //green
			}
			if ((18 <= rt1.width && rt1.width <= 25) && (18 <= rt1.height && rt1.height <= 25))//large
			{
				vRois_Large.push_back(rt1); //ū  ׸       
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

Point RectTL(Rect rect)
{
	Point rtPts[4] = { rect.tl(), Point(rect.tl().x + rect.width, rect.tl().y),rect.br(), Point(rect.tl().x, rect.br().y) };
	return rtPts[4];
}


// Function to find intersection between two vectors
template <typename T>
std::vector<T> findIntersection(const std::vector<T>& vec1, const std::vector<T>& vec2) {
	std::vector<T> intersection;
	for (const auto& elem : vec1) {
		if (std::find(vec2.begin(), vec2.end(), elem) != vec2.end()) {
			intersection.push_back(elem);
		}
	}
	return intersection;
}

void main()
{
	cv::Mat src_flawless = cv::imread(filePath_Search, cv::ImreadModes::IMREAD_GRAYSCALE);		//      ϼ ǰ
	cv::Mat src_gray_templt = cv::imread(filePath_Templt, cv::ImreadModes::IMREAD_GRAYSCALE);	//   ø 
	cv::Mat src_gray_search = cv::imread(filePath_Donut_Navy, cv::ImreadModes::IMREAD_GRAYSCALE);	//
	cv::Mat src_draw = cv::imread(filePath_Donut_Navy, cv::ImreadModes::IMREAD_ANYCOLOR);	//
	cv::Mat src_filled = src_draw.clone();


	vector<Rect> scratch_error, crack_error, contamination_finish_not;
	vector<Rect> contamination_error;
	//vector<int> scratchTL, crackTL, conTL;
	vector<Point> scratchTL, crackTL, conTL;
	vector<Point> scratchR, crackR, conR;

	//find objects
	vector<cv::Rect> vRois;
	Mat obj_Region;
	Point ptThrehold = Point(888, 150);
	double min_threshold = src_gray_search.data[ptThrehold.y * src_gray_search.cols + ptThrehold.x] + 5;
	cv::threshold(src_gray_search, obj_Region, min_threshold, 255, ThresholdTypes::THRESH_BINARY);
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	cv::findContours(obj_Region, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //  ü             ϱ         ü  ܰ        
	for (size_t k = 0; k < contours.size(); k++) //contourss.size();   ũ ⸸ŭ for           .(   Ӱ     ǵ    ü       (  ) 
	{
		double area = contourArea(contours[k]);

		RotatedRect rrt = minAreaRect(contours[k]);//  ü  ܰ                  ŭ          wRrt        
		Rect rt = rrt.boundingRect(); //   ü     ġ                   簢       

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
		scratchR.push_back(RectTL(scratch_error[e]));
	}
	//sort(scratchTL.begin(),scratchTL.end());

	crack_error = CrackDetect(vRois, src_gray_search, src_draw, src_filled);
	for (int w = 0; w < crack_error.size(); w++)
	{
		//crackTL.push_back(indexTL(crack_error[w])); 
		crackTL.push_back(pointTL(crack_error[w]));
		crackR.push_back(RectTL(crack_error[w]));
	}
	//sort(crackTL.begin(), crackTL.end());


	contamination_finish_not = ContaminationDetect(vRois, src_flawless, src_gray_search, src_draw, src_filled);
	for (int h = 0; h < contamination_finish_not.size(); h++)
	{
		//conTL.push_back(indexTL(contamination_finish_not[h])); 
		conTL.push_back(pointTL(contamination_finish_not[h]));
		conR.push_back(RectTL(contamination_finish_not[h]));
	}
	//sort(conTL.begin(), conTL.end());

	////////////////////////scratch, crack         ////////////////////////
	// scratchTL   conTL                
	// Find intersection between conTL and scratchTL
	std::vector<Point> intersectionScratch = findIntersection(conTL, scratchTL);

	// Remove intersection from conTL
	for (const auto& commonElem : intersectionScratch) {
		conTL.erase(std::remove_if(conTL.begin(), conTL.end(),
			[&commonElem](const Point& elem) {
				return elem.x == commonElem.x && elem.y == commonElem.y;
			}),
			conTL.end());
	}

	// Find intersection between conTL and crackTL
	std::vector<Point> intersectionCrack = findIntersection(conTL, crackTL);

	// Remove intersection from conTL
	for (const auto& commonElem : intersectionCrack) {
		conTL.erase(std::remove_if(conTL.begin(), conTL.end(),
			[&commonElem](const Point& elem) {
				return elem.x == commonElem.x && elem.y == commonElem.y;
			}),
			conTL.end());
	}

	int check11 = 100;

	//contamination_error:           contamination error      Rect       ;
	// conTl -> contamination error
	for (int s = 0; s < conTL.size(); s++)
	{
		Rect rt_s(conTL[s].x, conTL[s].y, 180, 182);
		contamination_error.push_back(rt_s);
	}



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


		cv::Point rtPts[4] = { contamination_error[x].tl(), Point(contamination_error[x].tl().x + 180, contamination_error[x].tl().y),
		contamination_error[x].br(), Point(contamination_error[x].tl().x, contamination_error[x].br().y) };

		for (size_t c = 0; c < 4; c++)
		{
			draw_line(src_draw, rtPts[c % 4], rtPts[(c + 1) % 4], color_con, 2, "", 3);
		}
	}

	int a1 = 0;

}
